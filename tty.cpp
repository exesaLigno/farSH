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
    size_t maximum_cursor_position = 0;

    Stack<size_t> cursor_positions_storage;

    bool lf_placed = false;

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
        FILE* current_stream = in;

        return UnicodeSymbol::Create([current_stream]() {
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

        int32_t cursor_position_shift = cursor_position - loaded_position;

        int32_t rows_shift = 0;
        int32_t columns_shift = cursor_position_shift <= cursor_position % terminalCols ? cursor_position_shift : cursor_position % terminalCols;
        cursor_position_shift -= columns_shift;

        rows_shift += cursor_position_shift / terminalCols + (cursor_position_shift % terminalCols != 0);
        if (cursor_position_shift % terminalCols != 0)
            columns_shift -= terminalCols - cursor_position_shift % terminalCols;

        MoveCursor(rows_shift, columns_shift);

        cursor_position = loaded_position;
    }

    void ClearLine()
    {
        bool size_was_changed = FetchSize();

        int32_t rows_shift = cursor_position / terminalCols;
        int32_t columns_shift = cursor_position % terminalCols;

        if (size_was_changed and cursor_position % terminalCols == 0 and not lf_placed and cursor_position == maximum_cursor_position)
        {
            rows_shift -= 1;
            columns_shift = terminalCols - 1;
        }

        else if (size_was_changed and lf_placed and cursor_position == maximum_cursor_position)
        {
            rows_shift += 1;
            columns_shift = 0;
        }

        MoveCursor(rows_shift, columns_shift);

        cursor_position = 0;
        maximum_cursor_position = 0;

        fputs("\e[J", out);
    }

    void Write(const UnicodeSymbol& symbol, bool last_symbol = false)
    {
        FetchSize();

        auto width = symbol.DisplayWidth();
        
        if ((cursor_position % terminalCols) + width > terminalCols)
        {
            cursor_position += terminalCols - cursor_position % terminalCols;
            maximum_cursor_position += terminalCols - cursor_position % terminalCols;
            fprintf(out, "\n");
        }

        cursor_position += width;
        maximum_cursor_position += width;
        symbol.WriteTo(out);
        lf_placed = false;

        if (cursor_position % terminalCols == 0 and width != 0 and last_symbol)
        {
            lf_placed = true;
            fputc('\n', out);
        }
    }

    void Reset()
    {
        cursor_position = 0;
        maximum_cursor_position = 0;
        cursor_positions_storage.Clear();
        lf_placed = false;
    }

    bool LFPlaced()
    {
        return lf_placed;
    }
};
