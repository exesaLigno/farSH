#pragma once

#include <cstdint>

namespace Multiprocessing
{
    class Status
    {
    public:
        enum class State : uint8_t
        {
            Pending, /// Process created but not started yet
            Running, /// Process is running
            Continued, /// Process is continued after SIGSTOP
            Exited, /// Process exited normally
            Terminated, /// Process terminated with signal
            Stopped, /// Process stopped with SIGSTOP
        };

    private:
        State state = State::Pending; /// Current state of the process

        /**
         * @brief Result of the process
         * @details Stores signal number for stopped or terminated process and 
         * return code for normally exited process.
         */
        int result = 0;

    public:
        Status();
        Status(const State _state);
        Status(const int _wstatus);

        Status& operator=(const State new_state);
        Status& operator=(const int wstatus);

        void UpdateWith(const State new_state);
        void UpdateWith(const int wstatus);

        State CurrentState() const;
        int ReturnCode() const;
        int Signal() const;

        const char* AsString() const;

        static const char* SignalName(int signal);
        static const char* RealtimeSignalName(int signal);
    };


    class ImmutableStatus : private Status
    {
    public:
        // ImmutableStatus can be constructed only of other ImmutableStatus object
        // or of existing Status object. It can't be constructed without any base.
        ImmutableStatus() = delete;
        ImmutableStatus(const ImmutableStatus& other) = default;
        ImmutableStatus& operator=(const ImmutableStatus& other) = default;

        ImmutableStatus(const Status& process_status);

        using Status::State;

        using Status::CurrentState;
        using Status::ReturnCode;
        using Status::Signal;

        using Status::AsString;
        
        using Status::SignalName;
        using Status::RealtimeSignalName;
    };
}
