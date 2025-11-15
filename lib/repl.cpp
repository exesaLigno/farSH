#include "repl.hpp"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctime>
#include <cstring>
#include <csignal>

uint64_t REPL::RollInfinitySidedDice()
{
    uint64_t result = (uint64_t) time(nullptr);
    result = (result ^ (result << 3)) | ~(result ^ (result >> 5));
    result = (rand() | ((uint64_t) rand()) << 32) ^ (result + 42);

    return result;
}

void REPL::Redraw(bool interactive)
{
    tty.Clear();
    UnicodeBuffer& outputBuffer = tty.OutputBuffer();
    outputBuffer.Clear();
    greeting.WriteTo(outputBuffer);
    printf("\e]2;%s\a", Greeting::GetWorkDir());

    if (infinitySidedDiceRollResult % 10 == 3 and not inputBuffer.Edited())
        outputBuffer.Append(" \e[3;90mrandom joke placeholder\e[0m");
    else
        outputBuffer.Append(inputBuffer, UnicodeBuffer::CursorMergePolicy::UseAppending);
    
    tty.Render(not interactive);
}

void REPL::Run()
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
                    inputBuffer.AssumeEdited();
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

                case UnicodeSymbol::Command::HorizontalTab:
                    break;

                default:
                    symbol.DebugWriteTo(stderr);
                    break;
            }
        }
        
        Redraw();
    }
}

int REPL::ExecuteCommand()
{
    if (inputBuffer.Length() == 0)
        return 0;
    char* string = inputBuffer.AsUnicodeString().ToString();
    auto ast = parser.Parse(string);

    FILE* fd = fopen("/tmp/farsh/last_command_ast.dot", "w");
    ast->DumpTo(fd);
    fclose(fd);

    interpreter.Execute(ast);

    delete ast;
    delete[] string;

    return 0;
}
