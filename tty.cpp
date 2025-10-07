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
#include "buffer.cpp"

class TTY
{
private:
    static TTY* instance;

    termios originalStdinAttributes;
    termios modifiedStdinAttributes;
    uint16_t terminalRows = 0;
    uint16_t terminalCols = 0;

    FILE* in = stdin;
    FILE* out = stdout;
    
    UnicodeBuffer outputBuffer;
    
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
    
    static void SignalHandler(int signo)
    {
        if (signo == SIGWINCH and instance)
            instance->Rerender();
        else if (signo == SIGINT and instance)
            ungetc('\x03', instance->in);
    }

public:
    TTY()
    {
        tcgetattr(STDIN_FILENO, &originalStdinAttributes);

        modifiedStdinAttributes = originalStdinAttributes;
        modifiedStdinAttributes.c_lflag &= ~(ECHO | ICANON);

        tcsetattr(STDIN_FILENO, TCSANOW, &modifiedStdinAttributes);
        FetchSize();
        
        instance = this;
        struct sigaction sa;
        sa.sa_handler = SignalHandler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGWINCH, &sa, NULL);
        sigaction(SIGINT, &sa, NULL);
    }

    ~TTY()
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &originalStdinAttributes);
    }

    UnicodeSymbol Fetch() const
    {
        FILE* current_stream = in;
        UnicodeSymbol res;

        UnicodeSymbol::Create(
            res,
            [current_stream]()
            {
                while (true)
                {
                    int byte = getc(current_stream);
                    if (byte >= 0)
                        return (Byte) byte;
                }
            }
        );
        
        return res;
    }

    void MoveCursor(int32_t rows_shift, int32_t columns_shift) const
    {
        if (columns_shift)
            fprintf(out, "\e[%d%c", abs(columns_shift), columns_shift > 0 ? 'D' : 'C');

        if (rows_shift)
            fprintf(out, "\e[%d%c", abs(rows_shift), rows_shift > 0 ? 'A' : 'B');
    }
    
    UnicodeBuffer& OutputBuffer()
    {
        return outputBuffer;
    }
    
    void Clear()
    {
        bool size_changed = FetchSize();
        
        size_t cursor_position = outputBuffer.WidthUntilCursor(terminalCols);
        int32_t rows_shift = cursor_position / terminalCols;
        int32_t columns_shift = cursor_position % terminalCols;
        
        if (size_changed and cursor_position % terminalCols == 0 and not lf_placed and outputBuffer.CursorAtTheEnd())
        {
            rows_shift -= 1;
            columns_shift = terminalCols - 1;
        }

        else if (size_changed and lf_placed and outputBuffer.CursorAtTheEnd())
        {
            rows_shift += 1;
            columns_shift = 0;
        }

        MoveCursor(rows_shift, columns_shift);
        fputs("\e[J", out);
    }
    
    void Render(bool no_cursor = false)
    {
        FetchSize();
        
        size_t line_end = outputBuffer.Width(terminalCols);
        size_t cursor_position = outputBuffer.WidthUntilCursor(terminalCols);
        
        outputBuffer.WriteTo(out);
        if (line_end % terminalCols == 0 or no_cursor)
        {
            lf_placed = true;
            fputc('\n', out);
        }
        
        if (no_cursor)
            return;
        
        int32_t cursor_position_shift = line_end - cursor_position;

        int32_t rows_shift = 0;
        int32_t columns_shift = cursor_position_shift <= line_end % terminalCols ? cursor_position_shift : line_end % terminalCols;
        cursor_position_shift -= columns_shift;

        rows_shift += cursor_position_shift / terminalCols + (cursor_position_shift % terminalCols != 0);
        if (cursor_position_shift % terminalCols != 0)
            columns_shift -= terminalCols - cursor_position_shift % terminalCols;

        MoveCursor(rows_shift, columns_shift);
    }
    
    void Rerender()
    {
        Clear();
        Render();
    }
};

TTY* TTY::instance = nullptr;
