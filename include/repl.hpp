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
#include "syntax_parser.hpp"
#include "interpreter.hpp"

class REPL
{
private:
    TTY tty;
    Greeting greeting;
    UnicodeBuffer inputBuffer;

    bool rudeMode = false;

    Parser parser;
    Interpreter interpreter;

    static uint64_t RollInfinitySidedDice();
    uint64_t infinitySidedDiceRollResult = RollInfinitySidedDice();

public:
    void Redraw(bool interactive = true);

    void Run();

    int ExecuteCommand();
};
