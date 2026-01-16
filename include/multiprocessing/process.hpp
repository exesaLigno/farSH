#pragma once

#include "status.hpp"

#include <cstdint>
#include <cstdlib>
#include <functional>

class Process
{
private:
    enum class Type : uint8_t { Unknown, Function };

    Type type = Type::Unknown;

    std::function<int()> internalTarget = nullptr;

    pid_t processId = 0;
    Status status;

    Process(const Process& other) = delete;
    Process& operator=(const Process& other) = delete;

public:
    Process(std::function<int()> target);

    Process(Process&& other) noexcept;
    Process& operator=(Process&& other) noexcept;

    void Spawn();
    void Join();
    void Kill();
    void Terminate();

    const ImmutableStatus CurrentStatus() const;

    // ~Process();
};
