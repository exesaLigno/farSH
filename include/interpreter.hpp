#pragma once

#include "syntax.hpp"

#include "stack.hpp"

class Interpreter
{
private:
    Stack<char*> stack;

    void ExecuteOperation(const WordOperation* word);
    void ExecuteOperation(const ArgumentOperation* argument);
    void ExecuteOperation(const InvocationOperation* invocation);

public:
    void Execute(const Operation* op);
};
