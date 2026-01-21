#pragma once

#include "status.hpp"

#include "flags.hpp"

#include <cstdlib>
#include <cstdint>
#include <functional>

namespace Multiprocessing
{
    class Process
    {
    private:
        enum class Type : uint8_t { Unknown, Function, Argv };

        Type type = Type::Unknown;

        void SetFlags(Flags flags);
        struct
        {
            bool captureSTDIN : 1 = false;
            bool captureSTDOUT : 1 = false;
            bool captureSTDERR : 1 = false;
        };

        union
        {
            std::function<int()> function = nullptr;
            char** argv;
        };

        pid_t processId = 0;
        Status status = Status::State::Pending;

        Process(const Process& other) = delete;
        Process& operator=(const Process& other) = delete;

    public:
        Process(std::function<int()> _function, Flags flags = Flags::Empty) noexcept;
        Process(const char* _prompt, Flags flags = Flags::Empty) = delete;
        Process(int _argc, const char* const _argv[], Flags flags = Flags::Empty) noexcept;

        Process(Process&& other) noexcept;
        Process& operator=(Process&& other) noexcept;

        void Spawn() noexcept;
        void Join() noexcept;
        void Kill() noexcept;
        void Terminate() noexcept;

        const ImmutableStatus CurrentStatus() const noexcept;

        ~Process() noexcept;
    };
}
