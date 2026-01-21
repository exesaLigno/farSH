#include "multiprocessing/status.hpp"

#include <sys/wait.h>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <stdexcept>

using namespace Multiprocessing;

Status::Status() { }

Status::Status(const State _state)
{
    UpdateWith(_state);
}

Status::Status(const int _wstatus)
{
    UpdateWith(_wstatus);
}

Status& Status::operator=(const State new_state)
{
    UpdateWith(new_state);

    return *this;
}

Status& Status::operator=(const int wstatus)
{
    UpdateWith(wstatus);

    return *this;
}

void Status::UpdateWith(const Status::State new_state)
{
    state = new_state;
}

void Status::UpdateWith(const int wstatus)
{
    if (WIFEXITED(wstatus))
    {
        state = State::Exited;
        result = WEXITSTATUS(wstatus);
    }

    else if (WIFSIGNALED(wstatus))
    {
        state = State::Terminated;
        result = WTERMSIG(wstatus);
    }

    else if (WIFSTOPPED(wstatus))
    {
        state = State::Stopped;
        result = WSTOPSIG(wstatus);
    }

    else if (WIFCONTINUED(wstatus))
    {
        state = State::Continued;
        result = 0;
    }

    else
    {
        state = State::Running;
        result = 0;
    }
}

Status::State Status::CurrentState() const
{
    return state;
}

int Status::ReturnCode() const
{
    return (state == State::Exited) ? result : -1;
}

int Status::Signal() const
{
    return (state == State::Terminated or state == State::Stopped) ? result : -1;
}

const char* Status::AsString() const
{
    static char retcode_representation[4] = { 0 };

    switch (state)
    {
        case State::Exited:
            sprintf(retcode_representation, "%d", ReturnCode());
            return retcode_representation;

        case State::Stopped:
        case State::Terminated:
            return SignalName(Signal());

        case State::Pending:
            return "Pending";

        case State::Running:
            return "Running";

        case State::Continued:
            return "Continued";

        default:
            throw std::runtime_error("Unreachable");
    }
}

const char* Status::SignalName(int signal)
{
    switch (signal)
    {
        case -1:
        case 0: 
            return "NO SIGNAL";
        
        case SIGHUP: return "SIGHUP";
        case SIGINT: return "SIGINT";
        case SIGQUIT: return "SIGQUIT";
        case SIGILL: return "SIGILL";
        case SIGTRAP: return "SIGTRAP";
        case SIGABRT: return "SIGABRT";
        case SIGBUS: return "SIGBUS";
        case SIGFPE: return "SIGFPE";
        case SIGKILL: return "SIGKILL";
        case SIGUSR1: return "SIGUSR1";
        case SIGSEGV: return "SIGSEGV";
        case SIGUSR2: return "SIGUSR2";
        case SIGPIPE: return "SIGPIPE";
        case SIGALRM: return "SIGALRM";
        case SIGTERM: return "SIGTERM";
        case SIGSTKFLT: return "SIGSTKFLT";
        case SIGCHLD: return "SIGCHLD";
        case SIGCONT: return "SIGCONT";
        case SIGSTOP: return "SIGSTOP";
        case SIGTSTP: return "SIGTSTP";
        case SIGTTIN: return "SIGTTIN";
        case SIGTTOU: return "SIGTTOU";
        case SIGURG: return "SIGURG";
        case SIGXCPU: return "SIGXCPU";
        case SIGXFSZ: return "SIGXFSZ";
        case SIGVTALRM: return "SIGVTALRM";
        case SIGPROF: return "SIGPROF";
        case SIGWINCH: return "SIGWINCH";
        case SIGIO: return "SIGIO";
        case SIGPWR: return "SIGPWR";
        case SIGSYS: return "SIGSYS";

        default:
            return RealtimeSignalName(signal);
    }
}

const char* Status::RealtimeSignalName(int signal)
{
    static char name[sizeof("SIGRTMIN+##")] = { 0 };
    if (signal == SIGRTMIN)
        strcpy(name, "SIGRTMIN");
    else
        sprintf(name, "SIGRTMIN%+d", signal - SIGRTMIN);
    return name;
}

ImmutableStatus::ImmutableStatus(const Status& process_status) : Status(process_status) { }
