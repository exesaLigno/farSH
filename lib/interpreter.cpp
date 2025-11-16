#include "interpreter.hpp"

#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

void Interpreter::Push(const char* value)
{
    char* copy = new char[strlen(value) + 1] { 0 };
    strcpy(copy, value);
    executionStack.Push(copy);
}

char* Interpreter::Pop()
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
    ResetProcessList();

    CheckAndExecute(operation);
    WaitAll();
}

void Interpreter::CheckAndExecute(const Operation* operation)
{
    switch (operation->Kind())
    {
        case OperationKind::Word:
            ExecuteWordOperation(operation);
            break;
        case OperationKind::Invocation:
            ExecuteInvocationOperation(operation);
            break;
        case OperationKind::EnvironmentVariableReference:
            ExecuteEnvironmentVariableReferenceOperation(operation);
            break;
        case OperationKind::Concatenation:
            ExecuteConcatenationOperation(operation);
            break;
        case OperationKind::PipeRedirection:
            ExecutePipeRedirectionOperation(operation);
            break;
        case OperationKind::FileRedirection:
            ExecuteFileRedirectionOperation(operation);
            break;
        case OperationKind::And:
            ExecuteAndOperation(operation);
            break;
        case OperationKind::Or:
            ExecuteOrOperation(operation);
            break;
        default:
            throw std::runtime_error("Trying to execute unsupported operation");
    }
}


void Interpreter::ExecuteWordOperation(const Operation* operation)
{
    Push(operation->As<WordOperation>()->GetText());
}

void Interpreter::ExecuteInvocationOperation(const Operation* operation)
{
    auto invocation = operation->As<InvocationOperation>();

    for (int idx = 0; idx < invocation->ChildrenCount(); idx++)
        CheckAndExecute(invocation->GetChild(idx));

    const int argc = invocation->ChildrenCount();
    char* argv[argc + 1];
    argv[argc] = 0;
    for (int idx = argc - 1; idx >= 0; idx--)
        argv[idx] = Pop();

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

void Interpreter::ExecuteEnvironmentVariableReferenceOperation(const Operation* operation)
{
    CheckAndExecute(operation->As<EnvironmentVariableLoadOperation>()->VariableName());

    char* name = Pop();
    char* value = getenv(name);

    delete[] name;

    Push(value ? value : "");
}

void Interpreter::ExecuteConcatenationOperation(const Operation* operation)
{
    auto composition = operation->As<ConcatenationOperation>();

    for (size_t idx = 0; idx < composition->ValuesCount(); idx++)
        CheckAndExecute(composition->Value(idx));

    const int composition_elements_count = composition->ChildrenCount();
    char* composition_elements[composition_elements_count + 1];
    composition_elements[composition_elements_count] = 0;

    size_t final_size = 0;

    for (int idx = composition_elements_count - 1; idx >= 0; idx--)
    {
        composition_elements[idx] = Pop();
        final_size += strlen(composition_elements[idx]);
    }

    char* result = new char[final_size + 1] { 0 };
    for (size_t idx = 0; idx < composition_elements_count; idx++)
        strcat(result, composition_elements[idx]);

    executionStack.Push(result);

    for (int idx = 0; idx < composition_elements_count; idx++)
        delete[] composition_elements[idx];
}

void Interpreter::ExecuteFileRedirectionOperation(const Operation* operation)
{
    auto file_redirection = operation->As<FileRedirectionOperation>();

    CheckAndExecute(file_redirection->Destination());

    char* filename = Pop();

    FILE* output = fopen(filename, file_redirection->HasFlag(FileRedirectionOperation::F_APPEND) ? "a" : "w");
    delete[] filename;

    if (file_redirection->HasFlag(FileRedirectionOperation::F_REDIRECT_STDOUT))
        dup2(fileno(output), STDOUT_FILENO);
    if (file_redirection->HasFlag(FileRedirectionOperation::F_REDIRECT_STDERR))
        dup2(fileno(output), STDERR_FILENO);

    CheckAndExecute(file_redirection->Source());

    fclose(output);
}

void Interpreter::ExecutePipeRedirectionOperation(const Operation* operation)
{
    auto pipe_redirection = operation->As<PipeRedirectionOperation>();

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

                Execute(pipe_redirection->GetOperand(idx));

            case ProcessKind::Parent:
                if (idx < pipe_redirection->OperandsCount() - 1)
                    close(pipes[idx][1]);
                if (idx > 0)
                    close(pipes[idx - 1][0]);
        }
    }

    WaitAll();
}

void Interpreter::ExecuteAndOperation(const Operation* operation)
{
    auto and_operation = operation->As<AndOperation>();

    CheckAndExecute(and_operation->First());

    if (CheckLastStatus() == 0)
        CheckAndExecute(and_operation->Second());
}

void Interpreter::ExecuteOrOperation(const Operation* operation)
{
    auto or_operation = operation->As<OrOperation>();

    CheckAndExecute(or_operation->First());

    if (CheckLastStatus() != 0)
        CheckAndExecute(or_operation->Second());
}

Interpreter::~Interpreter()
{
    childProcessCount = 0;

    if (childProcessList != nullptr)
        delete[] childProcessList;

    if (childProcessStatuses != nullptr)
        delete[] childProcessStatuses;
}
