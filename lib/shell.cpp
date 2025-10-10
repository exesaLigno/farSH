#include "shell.hpp"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctime>
#include <cstring>
#include <csignal>

uint64_t Shell::RollInfinitySidedDice()
{
    uint64_t result = (uint64_t) time(nullptr);
    result = (result ^ (result << 3)) | ~(result ^ (result >> 5));
    result = (rand() | ((uint64_t) rand()) << 32) ^ (result + 42);

    return result;
}

void Shell::Redraw(bool interactive)
{
    tty.Clear();
    UnicodeBuffer& outputBuffer = tty.OutputBuffer();
    outputBuffer.Clear();
    greeting.WriteTo(outputBuffer);
    printf("\e]2;%s\a", Greeting::GetWorkDir());

    if (infinitySidedDiceRollResult % 10 == 3 and inputBuffer.Length() == 0)
        outputBuffer.Append(" \e[3;90mrandom joke placeholder\e[0m");
    else
        outputBuffer.Append(inputBuffer, UnicodeBuffer::CursorMergePolicy::UseAppending);
    
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

                    infinitySidedDiceRollResult = RollInfinitySidedDice();

                    ExecuteCommand();
                    
                    inputBuffer.Clear();

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
