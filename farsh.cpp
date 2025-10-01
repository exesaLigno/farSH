#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctime>
#include <cstring>
#include <csignal>

#include "prompt.cpp"
#include "tty.cpp"
#include "buffer.cpp"

class FarSH
{
private:
    TTY tty;
    Prompt prompt;
    char cwd[2048] = { 0 };
    int inputStartRow = 0;
    int inputStartColumn = 0;
    UnicodeBuffer buffer;

    bool lfPlacedAlready = false;

public:
    void SetCursorPosition(const unsigned int row, const unsigned int column) const
    {
        printf("\e[%u;%uH", row, column);
    }

    void PrintAt(const unsigned int row, const unsigned int column, const char* text) const
    {
        printf("\e7\e[%u;%uH", row, column);
        printf(text);
        printf("\e8");
    }

    void Redraw()
    {
        tty.ClearLine();
        bool restore_cursor = false;
        for (size_t idx = 0; idx < buffer.bufferLength; idx++)
        {
            if (idx == buffer.displayCursorPosition)
            {
                restore_cursor = true;
                tty.StoreCursorPosition();
            }
            tty.Write(buffer[idx], idx == buffer.bufferLength - 1);
        }
        if (restore_cursor)
            tty.LoadCursorPosition();

        lfPlacedAlready = tty.LFPlaced();
    }

    void Run()
    {
        // prompt.Print();

        while (true)
        {
            UnicodeSymbol symbol = tty.Fetch();

            if (not symbol.IsCommand())
            {
                buffer.Insert(symbol);
                Redraw();
            }

            else
            {
                switch (symbol.GetCommand())
                {
                    case UnicodeSymbol::Command::LineFeed:
                        Redraw();
                        if (not lfPlacedAlready)
                            printf("\n");
                        buffer.Reset();
                        tty.Reset();
                        break;

                    case UnicodeSymbol::Command::Delete:
                        buffer.ClearSymbolBefore();
                        Redraw();
                        break;

                    case UnicodeSymbol::Command::DeleteAfter:
                        buffer.ClearSymbolAfter();
                        Redraw();
                        break;

                    case UnicodeSymbol::Command::EndOfTransmission:
                        buffer.Insert(" \e[3;33m...dying in agony...\e[0m");
                        Redraw();
                        if (not lfPlacedAlready)
                            printf("\n");
                        return;

                    case UnicodeSymbol::Command::CursorUp:
                    case UnicodeSymbol::Command::CursorDown:
                        /// TODO: Implement history search
                        break;

                    case UnicodeSymbol::Command::CursorForward:
                        buffer.MoveCursorForward();
                        Redraw();
                        break;

                    case UnicodeSymbol::Command::CursorBack:
                        buffer.MoveCursorBackward();
                        Redraw();
                        break;

                    default:
                        symbol.DebugWriteTo(stdout);
                        break;
                }
            }
        }
    }

    int ExecuteCommand(const char* command)
    {
        if (!strncmp(command, "cd", 2))
            chdir(command + 3);

        else
            printf("Command \"%s\" is not a program!\n", command);

        return 0;
    }
};