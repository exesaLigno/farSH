#include "shell.hpp"

#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctime>
#include <cstring>
#include <csignal>

void Shell::Redraw(bool interactive)
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
    
    tty.Render(not interactive);
}

void Shell::Run()
{
    Redraw();
    while (true)
    {
        UnicodeSymbol symbol = tty.Fetch();

        if (not symbol.IsCommand())
            inputBuffer.Insert(symbol);

        else
        {
            switch (symbol.GetCommand())
            {
                case UnicodeSymbol::Command::LineFeed:
                    Redraw(false);

                    ExecuteCommand();
                    
                    inputBuffer.Clear();
                    tty.Clear();

                    break;

                case UnicodeSymbol::Command::Delete:
                    inputBuffer.ClearSymbolBefore();
                    break;

                case UnicodeSymbol::Command::DeleteAfter:
                    inputBuffer.ClearSymbolAfter();
                    break;

                case UnicodeSymbol::Command::EndOfTransmission:
                    printf(" \e[3;33m...dying in agony...\e[0m\n");
                    return;

                case UnicodeSymbol::Command::CursorUp:
                case UnicodeSymbol::Command::CursorDown:
                    /// TODO: Implement history search
                    break;

                case UnicodeSymbol::Command::CursorForward:
                    inputBuffer.MoveCursorForward();
                    break;

                case UnicodeSymbol::Command::CursorBack:
                    inputBuffer.MoveCursorBackward();
                    break;
                
                case UnicodeSymbol::Command::CursorPosition:
                    inputBuffer.MoveCursorToStart();
                    break;
                    
                case UnicodeSymbol::Command::CursorPreviousLine:
                    inputBuffer.MoveCursorToEnd();
                    break;
                    
                case UnicodeSymbol::Command::EndOfText:
                    inputBuffer.Clear();
                    break;
                    
                case UnicodeSymbol::Command::DeviceControlThree:
                    inputBuffer.Prepend("sudo ");
                    break;

                default:
                    symbol.DebugWriteTo(stderr);
                    break;
            }
        }
        
        Redraw();
    }
}

int Shell::ExecuteCommand()
{
    printf("Executing command: '");
    inputBuffer.WriteTo();
    printf("'...\n");
    return 0;
}
