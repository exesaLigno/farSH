#include "interpreter.hpp"

#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

void Interpreter::Execute(const Operation* op)
{
    if (dynamic_cast<const WordOperation*>(op) != nullptr)
        ExecuteOperation(dynamic_cast<const WordOperation*>(op));
    else if (dynamic_cast<const InvocationOperation*>(op) != nullptr)
        ExecuteOperation(dynamic_cast<const InvocationOperation*>(op));
    else if (dynamic_cast<const EnvironmentVariableReferenceOperation*>(op) != nullptr)
        ExecuteOperation(dynamic_cast<const EnvironmentVariableReferenceOperation*>(op));
    else if (dynamic_cast<const CompositionOperation*>(op) != nullptr)
        ExecuteOperation(dynamic_cast<const CompositionOperation*>(op));
}

void Interpreter::ExecuteOperation(const WordOperation* word)
{
    char* word_str = new char[strlen(word->GetText()) + 1] { 0 };
    strcpy(word_str, word->GetText());
    stack.Push(word_str);
}

void Interpreter::ExecuteOperation(const InvocationOperation* invocation)
{
    for (int idx = 0; idx < invocation->ChildrenCount(); idx++)
        Execute(invocation->GetChild(idx));

    const int argc = invocation->ChildrenCount();
    char* argv[argc + 1];
    argv[argc] = 0;
    for (int idx = argc - 1; idx >= 0; idx--)
        argv[idx] = stack.Pop();

    pid_t child = fork();

    int wstatus = 0;

    if (!child)
    {
        execvp(argv[0], argv);
        abort();
    }

    else
    {
        wait(&wstatus);

        if (not WIFEXITED(wstatus))
            printf("farsh: \x1b[1;31mUnknown command\x1b[0m: %s\n", argv[0]);

        for (int idx = 0; idx < argc; idx++)
            delete[] argv[idx];
    }
}

void Interpreter::ExecuteOperation(const EnvironmentVariableReferenceOperation* environment_variable_reference)
{
    Execute(environment_variable_reference->VariableName());

    char* name = stack.Pop();
    char* value = getenv(name);

    if (value == nullptr)
        stack.Push(new char[1] { 0 });
    else
    {
        char* value_copy = new char[strlen(value) + 1] { 0 };
        strcpy(value_copy, value);
        stack.Push(value_copy);
    }
}

void Interpreter::ExecuteOperation(const CompositionOperation* composition)
{
    for (size_t idx = 0; idx < composition->ValuesCount(); idx++)
        Execute(composition->Value(idx));

    const int composition_elements_count = composition->ChildrenCount();
    char* composition_elements[composition_elements_count + 1];
    composition_elements[composition_elements_count] = 0;

    size_t final_size = 0;

    for (int idx = composition_elements_count - 1; idx >= 0; idx--)
    {
        composition_elements[idx] = stack.Pop();
        final_size += strlen(composition_elements[idx]);
    }

    char* result = new char[final_size + 1] { 0 };
    for (size_t idx = 0; idx < composition_elements_count; idx++)
        strcat(result, composition_elements[idx]);

    stack.Push(result);

    for (int idx = 0; idx < composition_elements_count; idx++)
        delete[] composition_elements[idx];
}
