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

    Stack<char*> executionStack;
    bool mainProcess = true;

    void ExecuteWordOperation(const Operation* word);
    void ExecuteRawStringLiteralOperation(const Operation* raw_string_literal);
    void ExecuteInvocationOperation(const Operation* invocation);
    void ExecuteEnvironmentVariableReferenceOperation(const Operation* environment_variable_reference);
    void ExecuteConcatenationOperation(const Operation* composition);
    void ExecuteFileRedirectionOperation(const Operation* file_redirection);
    void ExecutePipeRedirectionOperation(const Operation* pipe_redirection);

    Interpreter::ProcessKind Fork(pid_t& child_pid, bool just_ensure_not_main_process = false);

public:
    void Execute(const Operation* operation);
};
