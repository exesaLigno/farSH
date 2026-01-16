#include "multiprocessing/process.hpp"

#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>

#include <cstdio>

Process::Process(std::function<int()> target)
{
    internalTarget = target;
    type = Type::Function;
}

Process::Process(Process&& other) noexcept
{
    type = other.type;
    internalTarget = other.internalTarget;
    processId = other.processId;
    status = other.status;
}

Process& Process::operator=(Process&& other) noexcept
{
    if (this != &other)
    {
        type = other.type;
        internalTarget = other.internalTarget;
        processId = other.processId;
        status = other.status;
    }

    return *this;
}

void Process::Spawn()
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
            exit(internalTarget());
    }
}

void Process::Kill()
{
    kill(processId, SIGKILL);
    Join();
}

void Process::Terminate()
{
    kill(processId, SIGTERM);
    Join();
}

void Process::Join()
{
    int wstatus = 0;
    waitpid(processId, &wstatus, 0);
    status = wstatus;
}

const ImmutableStatus Process::CurrentStatus() const
{
    return status;
}
