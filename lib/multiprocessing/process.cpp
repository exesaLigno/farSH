#include "multiprocessing/process.hpp"

#include <cstdlib>
#include <sys/wait.h>
#include <cstring>

using namespace Multiprocessing;

void Process::SetFlags(Flags flags)
{
    if ((flags & Flags::CaptureSTDIN) == Flags::CaptureSTDIN)
        captureSTDIN = true;

    if ((flags & Flags::CaptureSTDOUT) == Flags::CaptureSTDOUT)
        captureSTDOUT = true;

    if ((flags & Flags::CaptureSTDERR) == Flags::CaptureSTDERR)
        captureSTDERR = true;
}

Process::Process(std::function<int()> _function, Flags flags) noexcept
{
    function = _function;
    type = Type::Function;
    SetFlags(flags);
}

Process::Process(const int _argc, const char* const _argv[], Flags flags) noexcept
{
    argv = new char*[_argc + 1] { nullptr };
    for (int i = 0; i < _argc; i++)
    {
        argv[i] = new char[strlen(_argv[i]) + 1] { 0 };
        strcpy(argv[i], _argv[i]);
    }

    type = Type::Argv;
    SetFlags(flags);
}

Process::Process(Process&& other) noexcept
{
    type = other.type;
    function = other.function;
    processId = other.processId;
    status = other.status;
}

Process& Process::operator=(Process&& other) noexcept
{
    if (this != &other)
    {
        type = other.type;
        function = other.function;
        processId = other.processId;
        status = other.status;
    }

    return *this;
}

void Process::Spawn() noexcept
{
    pid_t child_pid = fork();

    if (child_pid != 0)
    {
        status = Status::State::Running;
        processId = child_pid;
        return;
    }

    switch (type)
    {
        case Type::Function:
            exit(function());
        
        case Type::Argv:
            execvp(argv[0], argv);
            exit(1);
    }
}

void Process::Kill() noexcept
{
    kill(processId, SIGKILL);
    Join();
}

void Process::Terminate() noexcept
{
    kill(processId, SIGTERM);
    Join();
}

void Process::Join() noexcept
{
    int wstatus = 0;
    waitpid(processId, &wstatus, 0);
    status = wstatus;
}

const ImmutableStatus Process::CurrentStatus() const noexcept
{
    return status;
}

Process::~Process() noexcept
{
    switch (type)
    {
        case Type::Function:
            break;

        case Type::Argv:
            int i = 0;
            for (int i = 0; argv[i]; i++)
                delete[] argv[i];
            delete[] argv;
            break;
    }
}
