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
}

void Interpreter::ExecuteOperation(const WordOperation* word)
{
    // printf("executing word: %s\n", word->GetText());
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

        // else
        // 	this -> last_status = WEXITSTATUS(wstatus);
    }
}
