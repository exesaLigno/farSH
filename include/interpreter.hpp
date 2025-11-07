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

    Stack<char*> stack;
    bool alreadyForked = false;
    bool mainProcess = true;

    void ExecuteOperation(const WordOperation* word);
    void ExecuteOperation(const RawStringLiteralOperation* raw_string_literal);
    void ExecuteOperation(const InvocationOperation* invocation);
    void ExecuteOperation(const EnvironmentVariableReferenceOperation* environment_variable_reference);
    void ExecuteOperation(const ConcatenationOperation* composition);
    void ExecuteOperation(const FileRedirectionOperation* file_redirection);
    void ExecuteOperation(const PipeRedirectionOperation* pipe_redirection);

    Interpreter::ProcessKind Fork(pid_t& child_pid, bool just_ensure_not_main_process = false);

public:
    void Execute(const Operation* op);
};
