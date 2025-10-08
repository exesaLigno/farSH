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
#include "unicode_buffer.hpp"

class Shell
{
private:
    TTY tty;
    Greeting greeting;
    char cwd[2048] = { 0 };
    int inputStartRow = 0;
    int inputStartColumn = 0;
    UnicodeBuffer inputBuffer;

    UnicodeBuffer tmp_buffer;

    bool rudeMode = false;

public:
    void Redraw(bool interactive = true);

    void Run();

    int ExecuteCommand();
};
