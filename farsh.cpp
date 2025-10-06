#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctime>
#include <cstring>
#include <csignal>

#include "greeting.cpp"
#include "tty.cpp"
#include "buffer.cpp"

class FarSH
{
private:
    TTY tty;
    Greeting greeting;
    char cwd[2048] = { 0 };
    int inputStartRow = 0;
    int inputStartColumn = 0;
    UnicodeBuffer buffer;

    UnicodeBuffer tmp_buffer;

    bool lfPlacedAlready = false;

    bool rudeMode = false;

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

    void DrawGreeting()
    {
        tmp_buffer.Clear();
        greeting.WriteTo(tmp_buffer);
        for (size_t idx = 0; idx < tmp_buffer.Length(); idx++)
            tty.Write(tmp_buffer[idx], idx == buffer.Length() - 1);
        lfPlacedAlready = tty.LFPlaced();
    }

    void Redraw(bool interactive = true)
    {
        tty.ClearLine();
        DrawGreeting();
        bool restore_cursor = false;
        for (size_t idx = 0; idx < buffer.Length(); idx++)
        {
            if (idx == buffer.CursorPosition())
            {
                restore_cursor = true;
                tty.StoreCursorPosition();
            }
            tty.Write(buffer[idx], idx == buffer.Length() - 1);
        }
        if (restore_cursor)
            tty.LoadCursorPosition();

        lfPlacedAlready = tty.LFPlaced();
    }

    void Run()
    {
        Redraw();
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
                        Redraw(false);
                        if (not lfPlacedAlready)
                            printf("\n");

                        ExecuteCommand();
                        
                        buffer.Clear();
                        tty.Reset();

                        Redraw();
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

    int ExecuteCommand()
    {
        printf("Executing command: '");
        buffer.WriteTo();
        printf("'...\n");
        return 0;
    }
};
