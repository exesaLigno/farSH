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
    unsigned short int terminalXPixels = 0;
    unsigned short int terminalYPixels = 0;
    unsigned short int currentRow = 0;
    unsigned short int currentCol = 0;

    size_t line_length = 0;

    FILE* stream = stdout;

    bool FetchSize()
    {
        winsize size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
        
        bool changed = (
            terminalRows != size.ws_row ||
            terminalCols != size.ws_col ||
            terminalXPixels != size.ws_xpixel ||
            terminalYPixels != size.ws_ypixel
        );

        terminalRows = size.ws_row;
        terminalCols = size.ws_col;
        terminalXPixels = size.ws_xpixel;
        terminalYPixels = size.ws_ypixel;

        return changed;
    }

    static void test(int sig)
    {
        printf("hello");
    }

public:
    TTY()
    {
        std::signal(SIGWINCH, test);
        tcgetattr(STDIN_FILENO, &original_stdin_attributes);

        modified_stdin_attributes = original_stdin_attributes;
        modified_stdin_attributes.c_lflag &= ~(ECHO | ICANON);

        tcsetattr(STDIN_FILENO, TCSANOW, &modified_stdin_attributes);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
        FetchSize();
    }

    ~TTY()
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &original_stdin_attributes);
    }

    void MoveCursor(int shift)
    {
        int cur_col = currentCol + shift;
        int cur_row = currentRow;

        while (cur_col < 1)
        {
            cur_col += terminalCols;
            cur_row--;
        }
        
        while (cur_col > terminalCols)
        {
            cur_col -= terminalCols;
            cur_row++;
        }

        if (cur_row > terminalRows)
        {
            printf("\e[%dS", cur_row - terminalRows);
            cur_row = terminalRows;
        }

        if (cur_row < 1)
        {
            cur_row = 0;
        }

        printf("\e[%d;%dH", cur_row, cur_col);
    }

    Event Fetch()
    {
        if (FetchSize())
            return Event(Event::Type::WindowResize);

        enum class State
        {
            Start, End, Error,
            FSequence,
            CSI, CSIParameter, CSIInternal,
            nFSequence,
            Unicode2b1,
            Unicode3b1, Unicode3b2, 
            Unicode4b1, Unicode4b2, Unicode4b3,
        };

        /// FIXME: refactor for very long escape sequences!
        uint8_t buffer[100] = { 0 };
        size_t buffer_idx = 0;

        State current_state = State::Start;

        while (current_state != State::End and current_state != State::Error)
        {
            int byte = getc(stdin);
            
            if (byte == -1) // Ignoring empty input
                continue;

            switch (current_state)
            {
                case State::Start:
                    if ((byte >= 0x80 and byte <= 0xBF) or // Continuation bytes
                        (byte >= 0xC0 and byte <= 0xC1) or // Unused codes
                        (byte >= 0xF5 and byte <= 0xFF))   // Unused codes
                        current_state = State::Error;
                    else if ((byte >= 0x00 and byte <= 0x1A) or // Control codes
                             (byte >= 0x1C and byte <= 0x1F) or // Control codes
                             (byte == 0x7F) or                  // Control codes
                             (byte >= 0x20 and byte <= 0x7E))   // ASCII symbols
                        current_state = State::End;
                    else if (byte == 0x1B) // Escaped sequence
                        current_state = State::FSequence;
                    else if (byte >= 0xC2 and byte <= 0xDF) // Start of two-bytes unicode
                        current_state = State::Unicode2b1;
                    else if (byte >= 0xE0 and byte <= 0xEF) // Start of three-bytes unicode
                        current_state = State::Unicode3b1;
                    else if (byte >= 0xF0 and byte <= 0xF4) // Start of four-bytes unicode
                        current_state = State::Unicode4b1;
                    break;

                case State::FSequence:
                    if ((byte >= 0x00 and byte <= 0x1F) or // Incorrect for escape-sequence
                        (byte >= 0x7F and byte <= 0xFF) or // Incorrect for escape-sequence
                        (byte == 0x5D)) // FIXME: Operating system commands are not supported
                        current_state = State::Error;
                    else if ((byte >= 0x30 and byte <= 0x5A) or
                             (byte == 0x5C) or
                             (byte >= 0x5E and byte <= 0x7E))
                        current_state = State::End;
                    else if (byte == 0x5B)
                        current_state = State::CSI;
                    else if (byte >= 0x20 and byte <= 0x2F)
                        current_state = State::nFSequence;
                    break;
                
                case State::CSI:
                case State::CSIParameter:
                    if ((byte >= 0x00 and byte <= 0x1F) or
                        (byte == 0x7F) or
                        (byte >= 0x80 and byte <= 0xFF))
                        current_state = State::Error;
                    else if (byte >= 0x40 and byte <= 0x7E)
                        current_state = State::End;
                    else if (byte >= 0x30 and byte <= 0x3F)
                        current_state = State::CSIParameter;
                    else if (byte >= 0x20 and byte <= 0x2F)
                        current_state = State::CSIInternal;
                    break;
                
                case State::CSIInternal:
                    if ((byte >= 0x00 and byte <= 0x1F) or
                        (byte >= 0x30 and byte <= 0x3F) or
                        (byte == 0x7F) or
                        (byte >= 0x80 and byte <= 0xFF))
                        current_state = State::Error;
                    else if (byte >= 0x40 and byte <= 0x7E)
                        current_state = State::End;
                    else if (byte >= 0x20 and byte <= 0x2F)
                        current_state = State::CSIInternal;
                    break;
                
                case State::nFSequence:
                    if ((byte >= 0x00 and byte <= 0x1F) or
                        (byte == 0x7F) or
                        (byte >= 0x80 and byte <= 0xFF))
                        current_state = State::Error;
                    else if (byte >= 0x20 and byte <= 0x2F)
                        current_state = State::nFSequence;
                    else if (byte >= 0x30 and byte <= 0x7E)
                        current_state = State::End;                    
                    break;
                
                case State::Unicode2b1:
                    if ((byte >= 0x00 and byte <= 0x7F) or
                        (byte >= 0xC0 and byte <= 0xFF))
                        current_state = State::Error;
                    else
                        current_state = State::End;
                    break;
                
                case State::Unicode3b1:
                    if ((byte >= 0x00 and byte <= 0x7F) or
                        (byte >= 0xC0 and byte <= 0xFF))
                        current_state = State::Error;
                    else
                        current_state = State::Unicode3b2;
                    break;
                
                case State::Unicode3b2:
                    if ((byte >= 0x00 and byte <= 0x7F) or
                        (byte >= 0xC0 and byte <= 0xFF))
                        current_state = State::Error;
                    else
                        current_state = State::End;
                    break;
                
                case State::Unicode4b1:
                    if ((byte >= 0x00 and byte <= 0x7F) or
                        (byte >= 0xC0 and byte <= 0xFF))
                        current_state = State::Error;
                    else
                        current_state = State::Unicode4b2;
                    break;
                
                case State::Unicode4b2:
                    if ((byte >= 0x00 and byte <= 0x7F) or
                        (byte >= 0xC0 and byte <= 0xFF))
                        current_state = State::Error;
                    else
                        current_state = State::Unicode4b3;
                    break;
                
                case State::Unicode4b3:
                    if ((byte >= 0x00 and byte <= 0x7F) or
                        (byte >= 0xC0 and byte <= 0xFF))
                        current_state = State::Error;
                    else
                        current_state = State::End;
                    break;
                
            }

            buffer[buffer_idx++] = byte;
        }

        Event ev = Event(buffer, buffer_idx);

        if (ev.GetSymbolType() == UnicodeSymbol::Type::EscapedSequence and ev.GetCommand() == UnicodeSymbol::Command::CursorPosition)
        {
            currentRow = ev.GetCSIParameter(0);
            currentCol = ev.GetCSIParameter(1);
            return Fetch();
        }

        else
            return Event(buffer, buffer_idx);
    }

    void ResetLine()
    {
        line_length = 0;
    }
};
