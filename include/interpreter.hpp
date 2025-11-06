#pragma once

#include "syntax.hpp"

#include "stack.hpp"

class Interpreter
{
private:
    Stack<char*> stack;

    void ExecuteOperation(const WordOperation* word);
    void ExecuteOperation(const InvocationOperation* invocation);
    void ExecuteOperation(const EnvironmentVariableReferenceOperation* environment_variable_reference);
    void ExecuteOperation(const CompositionOperation* composition);

public:
    void Execute(const Operation* op);
};
