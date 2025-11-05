#pragma once

#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctime>
#include <cstring>
#include <csignal>

#include "greeting.hpp"
#include "tty.hpp"
#include "unicode/buffer.hpp"

class REPL
{
private:
    TTY tty;
    Greeting greeting;
    UnicodeBuffer inputBuffer;

    bool rudeMode = false;

    static uint64_t RollInfinitySidedDice();
    uint64_t infinitySidedDiceRollResult = RollInfinitySidedDice();

public:
    void Redraw(bool interactive = true);

    void Run();

    int ExecuteCommand();
};
