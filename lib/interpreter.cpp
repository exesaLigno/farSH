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
        return ProcessKind::Parent;
}

void Interpreter::Execute(const Operation* operation)
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
        default:
            throw std::runtime_error("Trying to execute unsupported operation");
    }
}


void Interpreter::ExecuteWordOperation(const Operation* operation)
{
    Push(operation->As<WordOperation>()->GetText());
}

void Interpreter::ExecuteRawStringLiteralOperation(const Operation* operation)
{
    Push(operation->As<RawStringLiteralOperation>()->GetText());
}

void Interpreter::ExecuteInvocationOperation(const Operation* operation)
{
    auto invocation = operation->As<InvocationOperation>();

    for (int idx = 0; idx < invocation->ChildrenCount(); idx++)
        Execute(invocation->GetChild(idx));

    const int argc = invocation->ChildrenCount();
    char* argv[argc + 1];
    argv[argc] = 0;
    for (int idx = argc - 1; idx >= 0; idx--)
        argv[idx] = Pop();

    pid_t child_pid;
    int wstatus = 0;

    switch (Fork(child_pid, true))
    {
        case ProcessKind::Child:
            execvp(argv[0], argv);
            printf("farsh: \x1b[1;31mUnknown command\x1b[0m: %s\n", argv[0]);
            exit(0);

        case ProcessKind::Parent:
            wait(&wstatus);
    }

    for (int idx = 0; idx < argc; idx++)
        delete[] argv[idx];
}

void Interpreter::ExecuteEnvironmentVariableReferenceOperation(const Operation* operation)
{
    Execute(operation->As<EnvironmentVariableLoadOperation>()->VariableName());

    char* name = Pop();
    char* value = getenv(name);

    Push(value ? value : "");
}

void Interpreter::ExecuteConcatenationOperation(const Operation* operation)
{
    auto composition = operation->As<ConcatenationOperation>();

    for (size_t idx = 0; idx < composition->ValuesCount(); idx++)
        Execute(composition->Value(idx));

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

    Execute(file_redirection->Destination());

    char* filename = Pop();

    FILE* output = fopen(filename, file_redirection->HasFlag(FileRedirectionOperation::F_APPEND) ? "a" : "w");
    delete[] filename;

    if (file_redirection->HasFlag(FileRedirectionOperation::F_REDIRECT_STDOUT))
        dup2(fileno(output), STDOUT_FILENO);
    if (file_redirection->HasFlag(FileRedirectionOperation::F_REDIRECT_STDERR))
        dup2(fileno(output), STDERR_FILENO);

    Execute(file_redirection->Source());

    fclose(output);
}

void Interpreter::ExecutePipeRedirectionOperation(const Operation* operation)
{
    auto pipe_redirection = operation->As<PipeRedirectionOperation>();

    pid_t child_pid;
    int wstatus = 0;

    switch (Fork(child_pid))
    {
        case ProcessKind::Child:
            int pipe_fd[2];
            pipe(pipe_fd);

            switch (Fork(child_pid))
            {
                case ProcessKind::Parent:
                    close(pipe_fd[0]);
                    dup2(pipe_fd[1], STDOUT_FILENO);
                    close(pipe_fd[1]);

                    Execute(pipe_redirection->Source());
                    exit(0);

                case ProcessKind::Child:
                    close(pipe_fd[1]);
                    dup2(pipe_fd[0], STDIN_FILENO);
                    close(pipe_fd[0]);

                    Execute(pipe_redirection->Destination());
                    exit(0);
            }

            break;

        case ProcessKind::Parent:
            wait(&wstatus);
    }
}
