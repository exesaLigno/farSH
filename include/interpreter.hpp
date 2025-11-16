#pragma once

#include "syntax.hpp"

#include "stack.hpp"

class Interpreter
{
private:
    enum class ProcessKind
    {
        Child, Parent
    };

    enum SpecialStatuses
    {
        Running = -1,
    };

    Stack<char*> executionStack;
    bool mainProcess = true;

    int childProcessCount = 0;
    pid_t* childProcessList = nullptr;
    int* childProcessStatuses = nullptr;

    void ResetProcessList();
    void AddProcessToList(const pid_t pid);
    void AddProcessStatus(const pid_t pid, const int status);
    int CheckLastStatus() const;

    void Push(const char* value);
    char* Pop();

    void CheckAndExecute(const Operation* operation);

    void ExecuteWordOperation(const Operation* word);
    void ExecuteRawStringLiteralOperation(const Operation* raw_string_literal);
    void ExecuteInvocationOperation(const Operation* invocation);
    void ExecuteEnvironmentVariableReferenceOperation(const Operation* environment_variable_reference);
    void ExecuteConcatenationOperation(const Operation* composition);
    void ExecuteFileRedirectionOperation(const Operation* file_redirection);
    void ExecutePipeRedirectionOperation(const Operation* pipe_redirection);
    void ExecuteAndOperation(const Operation* and_operation);
    void ExecuteOrOperation(const Operation* or_operation);

    Interpreter::ProcessKind Fork(pid_t& child_pid, bool just_ensure_not_main_process = false);
    void WaitAll();

public:
    ~Interpreter();

    void Execute(const Operation* operation);
};
