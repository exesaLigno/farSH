#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <csignal>
#include <cstdio>

#include "event.cpp"
#include "string.cpp"

class TTY
{
private:
    termios original_stdin_attributes;
    termios modified_stdin_attributes;
    uint16_t terminalRows = 0;
    uint16_t terminalCols = 0;

    UnicodeString dynamicString;

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

    static int32_t abs(int32_t value)
    {
        int32_t mask = value >> 31;
        return (value ^ mask) - mask;
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

    UnicodeString& DynamicString()
    {
        return dynamicString;
    }

    void ClearDynamicString()
    {
        FetchSize();
        size_t rows_up = dynamicString.DisplayWidth() / terminalCols;
        size_t columns_back = dynamicString.DisplayWidth() % terminalCols;
        
        if (rows_up != 0)
            fprintf(stream, "\e[%dA", rows_up);
        if (columns_back != 0)
            fprintf(stream, "\e[%dD", columns_back);

        fprintf(stream, "\e[J");
    }

    void PrintDynamicString()
    {
        dynamicString.WriteTo(stream);

        FetchSize();

        size_t target_row = dynamicString.DisplaCursorPosition() / terminalCols;
        size_t target_column = dynamicString.DisplaCursorPosition() % terminalCols;

        size_t current_row = dynamicString.DisplayWidth() / terminalCols;
        size_t current_column = dynamicString.DisplayWidth() % terminalCols;

        int rows_shift = target_row - current_row;
        int columns_shift = target_column - current_column;

        if (rows_shift != 0)
            fprintf(stream, "\e%d%c", abs(rows_shift), rows_shift < 0 ? 'A' : 'B');
        if (columns_shift != 0)
            fprintf(stream, "\e%d%c", abs(columns_shift), columns_shift < 0 ? 'D' : 'C');
    }
};
