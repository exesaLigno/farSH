#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <csignal>
#include <cstdio>

#include "event.cpp"
#include "buffer.cpp"

class TTY
{
private:
    termios original_stdin_attributes;
    termios modified_stdin_attributes;
    unsigned short int terminalRows = 0;
    unsigned short int terminalCols = 0;

    FILE* stream = stdout;

    bool FetchSize()
    {
        winsize size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
        
        bool changed = (
            terminalRows != size.ws_row ||
            terminalCols != size.ws_col
        );

        terminalRows = size.ws_row;
        terminalCols = size.ws_col;

        return changed;
    }

public:
    TTY()
    {
        tcgetattr(STDIN_FILENO, &original_stdin_attributes);

        modified_stdin_attributes = original_stdin_attributes;
        modified_stdin_attributes.c_lflag &= ~(ECHO | ICANON);

        tcsetattr(STDIN_FILENO, TCSANOW, &modified_stdin_attributes);
        FetchSize();
    }

    ~TTY()
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &original_stdin_attributes);
    }

    UnicodeSymbol Fetch()
    {
        FetchSize();

        FILE* current_stream = stream;

        return UnicodeSymbol::CreateFromStream([current_stream]() {
            while (true)
            {
                int byte = getc(current_stream);
                if (byte >= 0)
                    return (Byte) byte;
            }
        });
    }
};
