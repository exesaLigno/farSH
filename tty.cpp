#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <csignal>
#include <cstdio>
#include <cstdint>

#include "stack.hpp"
#include "symbol.cpp"

class TTY
{
private:
    termios original_stdin_attributes;
    termios modified_stdin_attributes;
    uint16_t terminalRows = 0;
    uint16_t terminalCols = 0;

    FILE* in = stdin;
    FILE* out = stdout;

    size_t cursor_position = 0;

    Stack<size_t> cursor_positions_storage;

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
        
        FILE* current_stream = in;

        return UnicodeSymbol::CreateFromStream([current_stream]() {
            while (true)
            {
                int byte = getc(current_stream);
                if (byte >= 0)
                    return (Byte) byte;
            }
        });
    }

    void MoveCursor(int32_t rows_shift, int32_t columns_shift)
    {
        if (columns_shift)
            fprintf(out, "\e[%d%c", abs(columns_shift), columns_shift > 0 ? 'D' : 'C');

        if (rows_shift)
            fprintf(out, "\e[%d%c", abs(rows_shift), rows_shift > 0 ? 'A' : 'B');
    }

    void StoreCursorPosition()
    {
        cursor_positions_storage.Push(cursor_position);
    }

    void LoadCursorPosition()
    {
        size_t loaded_position = cursor_positions_storage.Pop();

        /// TODO: Implement shift calculation
        int32_t rows_shift = 0;
        int32_t columns_shift = 0;

        MoveCursor(rows_shift, columns_shift);

        cursor_position = loaded_position;
    }

    void ClearLine()
    {
        FetchSize();

        int32_t rows_shift = cursor_position / terminalCols;
        int32_t columns_shift = cursor_position % terminalCols;

        MoveCursor(rows_shift, columns_shift);

        cursor_position = 0;

        fputs("\e[J", out);
    }

    void Write(const UnicodeSymbol& symbol, bool last_symbol = false)
    {
        FetchSize();

        auto width = symbol.DisplayWidth();
        
        if ((cursor_position % terminalCols) + width > terminalCols)
        {
            cursor_position += terminalCols - cursor_position % terminalCols;
            fprintf(out, "\n");
        }

        cursor_position += symbol.DisplayWidth();
        symbol.WriteTo(out);

        if (cursor_position % terminalCols == 0 and last_symbol)
            fprintf(out, "\n");
    }
};
