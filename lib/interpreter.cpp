#include "interpreter.hpp"

#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

void Interpreter::MemoryPush(const char* value)
{
    char* copy = new char[strlen(value) + 1] { 0 };
    strcpy(copy, value);
    executionStack.Push(copy);
}

char* Interpreter::MemoryPop()
{
    return executionStack.Pop();
}

void Interpreter::ResetProcessList()
{
    childProcessCount = 0;

    if (childProcessList != nullptr)
        delete[] childProcessList;

    if (childProcessStatuses != nullptr)
        delete[] childProcessStatuses;

    childProcessList = nullptr;
    childProcessStatuses = nullptr;
}

void Interpreter::AddProcessToList(const pid_t pid)
{
    if (childProcessCount == 0)
    {
        childProcessCount++;
        childProcessList = new pid_t[childProcessCount] { 0 };
        childProcessStatuses = new int[childProcessCount] { Running };
    }

    else
    {
        int old_count = childProcessCount++;
        
        pid_t* new_child_process_list = new pid_t[childProcessCount] { 0 };
        int* new_child_process_statuses = new int[childProcessCount] { Running };

        memcpy(new_child_process_list, childProcessList, old_count * sizeof(pid_t));
        memcpy(new_child_process_statuses, childProcessStatuses, old_count * sizeof(int));

        delete[] childProcessList;
        delete[] childProcessStatuses;

        childProcessList = new_child_process_list;
        childProcessStatuses = new_child_process_statuses;
    }

    childProcessList[childProcessCount - 1] = pid;
}

void Interpreter::AddProcessStatus(const pid_t pid, const int status)
{
    for (int idx = 0; idx < childProcessCount; idx++)
        if (childProcessList[idx] == pid)
        {
            childProcessStatuses[idx] = status;
            break;
        }
}

int Interpreter::CheckLastStatus() const
{
    return childProcessStatuses[childProcessCount - 1];
}

Interpreter::ProcessKind Interpreter::Fork(pid_t& child_pid, bool just_ensure_not_main_process)
{
    child_pid = 0;

    if (just_ensure_not_main_process and not mainProcess)
        return ProcessKind::Child;

    child_pid = fork();

    if (child_pid == 0)
    {
        mainProcess = false;
        return ProcessKind::Child;
    }

    else
    {
        AddProcessToList(child_pid);
        return ProcessKind::Parent;
    }
}

void Interpreter::WaitAll()
{
    pid_t child_pid = 0;
    int wstatus;

    while ((child_pid = wait(&wstatus)) > 0)
        AddProcessStatus(child_pid, wstatus);
}

void Interpreter::Execute(const Operation* operation)
{
#ifdef DEBUG
    FILE* fd = fopen("/tmp/farsh/last_command_ast.dot", "w");
    operation->DumpTo(fd);
    fclose(fd);
#endif

    ResetProcessList();

    ExecuteOperation(operation);
    WaitAll();
}

void Interpreter::ExecuteOperation(const Operation* operation)
{
    switch (operation->Kind())
    {
        case OperationKind::Word:
            ExecuteWordOperation(operation->As<WordOperation>());
            break;
        case OperationKind::Invocation:
            ExecuteInvocationOperation(operation->As<InvocationOperation>());
            break;
        case OperationKind::EnvironmentVariableLoad:
            ExecuteEnvironmentVariableLoadOperation(operation->As<EnvironmentVariableLoadOperation>());
            break;
        case OperationKind::Concatenation:
            ExecuteConcatenationOperation(operation->As<ConcatenationOperation>());
            break;
        case OperationKind::PipeRedirection:
            ExecutePipeRedirectionOperation(operation->As<PipeRedirectionOperation>());
            break;
        case OperationKind::FileRedirection:
            ExecuteFileRedirectionOperation(operation->As<FileRedirectionOperation>());
            break;
        case OperationKind::And:
            ExecuteAndOperation(operation->As<AndOperation>());
            break;
        case OperationKind::Or:
            ExecuteOrOperation(operation->As<OrOperation>());
            break;
        default:
            throw std::runtime_error("Trying to execute unsupported operation");
    }
}

void Interpreter::ExecuteDescendants(const Operation* operation)
{
    for (int idx = 0; idx < operation->ChildrenCount(); idx++)
        ExecuteOperation(operation->GetChild(idx));
}

void Interpreter::ExecuteWordOperation(const WordOperation* word)
{
    MemoryPush(word->GetText());
}

void Interpreter::ExecuteInvocationOperation(const InvocationOperation* invocation)
{
    ExecuteDescendants(invocation);

    const int argc = invocation->ChildrenCount();
    char* argv[argc + 1];
    argv[argc] = 0;
    for (int idx = argc - 1; idx >= 0; idx--)
        argv[idx] = MemoryPop();

    if (!strcmp(argv[0], "cd"))
    {
        chdir(argv[1]);
        return;
    }

    pid_t child_pid;
    int wstatus = 0;

    switch (Fork(child_pid, true))
    {
        case ProcessKind::Child:
            execvp(argv[0], argv);
            printf("farsh: \x1b[1;31mUnknown command\x1b[0m: %s\n", argv[0]);
            exit(0);

        case ProcessKind::Parent:
            WaitAll();
    }

    for (int idx = 0; idx < argc; idx++)
        delete[] argv[idx];
}

void Interpreter::ExecuteEnvironmentVariableLoadOperation(const EnvironmentVariableLoadOperation* environment_variable_load)
{
    ExecuteOperation(environment_variable_load->VariableName());

    char* name = MemoryPop();
    char* value = getenv(name);

    delete[] name;

    MemoryPush(value ? value : "");
}

void Interpreter::ExecuteConcatenationOperation(const ConcatenationOperation* concatenation)
{
    ExecuteDescendants(concatenation);

    const int composition_elements_count = concatenation->ChildrenCount();
    char* composition_elements[composition_elements_count + 1];
    composition_elements[composition_elements_count] = 0;

    size_t final_size = 0;

    for (int idx = composition_elements_count - 1; idx >= 0; idx--)
    {
        composition_elements[idx] = MemoryPop();
        final_size += strlen(composition_elements[idx]);
    }

    char* result = new char[final_size + 1] { 0 };
    for (size_t idx = 0; idx < composition_elements_count; idx++)
        strcat(result, composition_elements[idx]);

    executionStack.Push(result);

    for (int idx = 0; idx < composition_elements_count; idx++)
        delete[] composition_elements[idx];
}

void Interpreter::ExecuteFileRedirectionOperation(const FileRedirectionOperation* file_redirection)
{
    ExecuteOperation(file_redirection->Filename());

    char* filename = MemoryPop();

    FILE* output = fopen(filename, file_redirection->HasFlag(FileRedirectionOperation::F_APPEND) ? "a" : "w");
    delete[] filename;

    if (file_redirection->HasFlag(FileRedirectionOperation::F_REDIRECT_STDOUT))
        dup2(fileno(output), STDOUT_FILENO);
    if (file_redirection->HasFlag(FileRedirectionOperation::F_REDIRECT_STDERR))
        dup2(fileno(output), STDERR_FILENO);

    ExecuteOperation(file_redirection->Source());

    fclose(output);
}

void Interpreter::ExecutePipeRedirectionOperation(const PipeRedirectionOperation* pipe_redirection)
{
    int pipes_count = pipe_redirection->OperandsCount() - 1;

    int pipes[pipes_count][2];

    for (int pipe_idx = 0; pipe_idx < pipes_count; pipe_idx++)
        pipe(pipes[pipe_idx]);

    for (int idx = 0; idx < pipe_redirection->OperandsCount(); idx++)
    {
        pid_t child_pid;
        switch (Fork(child_pid))
        {
            case ProcessKind::Child:
                if (idx < pipe_redirection->OperandsCount() - 1)
                {
                    close(pipes[idx][0]);
                    dup2(pipes[idx][1], STDOUT_FILENO);
                    close(pipes[idx][1]);
                }

                if (idx > 0)
                {
                    close(pipes[idx - 1][1]);
                    dup2(pipes[idx - 1][0], STDIN_FILENO);
                    close(pipes[idx - 1][0]);
                }

                ExecuteOperation(pipe_redirection->GetOperand(idx));

            case ProcessKind::Parent:
                if (idx < pipe_redirection->OperandsCount() - 1)
                    close(pipes[idx][1]);
                if (idx > 0)
                    close(pipes[idx - 1][0]);
        }
    }

    WaitAll();
}

void Interpreter::ExecuteAndOperation(const AndOperation* and_operation)
{
    ExecuteOperation(and_operation->First());

    if (CheckLastStatus() == 0)
        ExecuteOperation(and_operation->Second());
}

void Interpreter::ExecuteOrOperation(const OrOperation* or_operation)
{
    ExecuteOperation(or_operation->First());

    if (CheckLastStatus() != 0)
        ExecuteOperation(or_operation->Second());
}

Interpreter::~Interpreter()
{
    childProcessCount = 0;

    if (childProcessList != nullptr)
        delete[] childProcessList;

    if (childProcessStatuses != nullptr)
        delete[] childProcessStatuses;
}
