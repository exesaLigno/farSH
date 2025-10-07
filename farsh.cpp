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
    UnicodeBuffer inputBuffer;

    UnicodeBuffer tmp_buffer;

    bool lfPlacedAlready = false;

    bool rudeMode = false;

public:
    void Redraw(bool interactive = true)
    {
        tty.Clear();
        UnicodeBuffer& outputBuffer = tty.OutputBuffer();
        outputBuffer.Clear();
        greeting.WriteTo(outputBuffer);
        bool cursor_set = false;
        for (size_t idx = 0; idx < inputBuffer.Length(); idx++)
        {            
            if (idx == inputBuffer.CursorPosition())
            {
                outputBuffer.SetCursor();
                cursor_set = true;
            }
            
            outputBuffer.Append(inputBuffer[idx]);
        }
        if (not cursor_set)
            outputBuffer.SetCursor();
        
        tty.Render();
    }

    void Run()
    {
        Redraw();
        while (true)
        {
            UnicodeSymbol symbol = tty.Fetch();

            if (not symbol.IsCommand())
            {
                inputBuffer.Insert(symbol);
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
                        
                        inputBuffer.Clear();
                        tty.Clear();

                        Redraw();
                        break;

                    case UnicodeSymbol::Command::Delete:
                        inputBuffer.ClearSymbolBefore();
                        Redraw();
                        break;

                    case UnicodeSymbol::Command::DeleteAfter:
                        inputBuffer.ClearSymbolAfter();
                        Redraw();
                        break;

                    case UnicodeSymbol::Command::EndOfTransmission:
                        inputBuffer.Insert(" \e[3;33m...dying in agony...\e[0m");
                        Redraw();
                        if (not lfPlacedAlready)
                            printf("\n");
                        return;

                    case UnicodeSymbol::Command::CursorUp:
                    case UnicodeSymbol::Command::CursorDown:
                        /// TODO: Implement history search
                        break;

                    case UnicodeSymbol::Command::CursorForward:
                        inputBuffer.MoveCursorForward();
                        Redraw();
                        break;

                    case UnicodeSymbol::Command::CursorBack:
                        inputBuffer.MoveCursorBackward();
                        Redraw();
                        break;
                        
                    case UnicodeSymbol::Command::EndOfText:
                        inputBuffer.Clear();
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
        inputBuffer.WriteTo();
        printf("'...\n");
        return 0;
    }
};
