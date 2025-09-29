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

    // void PrintHeader()
    // {
    //     char header[terminalCols + 10];
    //     for (int i = 0; i < terminalCols + 10; i++)
    //         header[i] = ' ';
    //     header[terminalCols + 9] = 0;
    //     int r = sprintf(header + 5, " Timestamp is %d! ", (int) time(NULL));
    //     header[5 + r] = ' ';
    //     strncpy(header, "\e[44m", 5);
    //     strncpy(header + 5 + terminalCols, "\e[0m", 4);
    //     PrintAt(0, 0, header);
    // }

    // void FillDynamicString(const UnicodeBuffer& buffer, UnicodeString& dynamic_string, bool completing = true)
    // {

    // }
    void Redraw(int signum = 0)
    {
        tty.ClearLine();
        bool restore_cursor = false;
        for (size_t idx = 0; idx < buffer.bufferLength; idx++)
        {
            tty.Write(buffer[idx], idx == buffer.bufferLength - 1);
            if (idx == buffer.displayCursorPosition)
            {
                restore_cursor = true;
                tty.StoreCursorPosition();
            }
        }
        if (restore_cursor)
            tty.LoadCursorPosition();
    }

    void Run()
    {
        // prompt.Print();

        while (true)
        {
            UnicodeSymbol symbol = tty.Fetch();
            
            // printf("%d", symbol.DisplayWidth());
            // continue;

            // if (event.Is(Event::Type::Input) and event.GetSymbol().IsCorrect())
            //     event.GetSymbol().WriteTo(stdout);

            if (not symbol.IsCommand())
            {
                // printf("%c", symbol.GetByte(0));
                buffer.Insert(symbol);
                Redraw();
            }

            // else
            // {
            //     switch (event.GetCommand())
            //     {
            //         case UnicodeSymbol::Command::LineFeed:
            //             tty.ClearDynamicString();
            //             FillDynamicString(buffer, tty.DynamicString(), true);
            //             tty.PrintDynamicString();
            //             printf("\n");
            //             buffer.Reset();
            //             fprintf(stdout, "\e[0J");
            //             break;

            //         case UnicodeSymbol::Command::Delete:
            //             // size_t old_pos = buffer.current_position;
            //             // tty.Clear(buffer);
            //             // tty.UpdateOutput(buffer, buffer.ClearSymbolBefore());
            //             // // buffer.ClearSymbolBefore();
            //             // printf("\e[%d;%dH", inputStartRow, inputStartColumn);
            //             // printf("\e[0J");
            //             // buffer.WriteTo();
            //             // tty.MoveCursor(buffer.current_position, inputStartRow, inputStartColumn);
            //             break;

            //         // case Event::Command::Stop:
            //         //     printf("Buy!\n");
            //         //     break;

            //         // case Event::Command::ArrowUp:
            //         //     printf("↑");
            //         //     break;

            //         // case Event::Command::ArrowDown:
            //         //     printf("↓");
            //         //     break;

            //         case UnicodeSymbol::Command::CursorForward:
            //             int display_shift = buffer.MoveCursorForward();
            //             break;

            //         case UnicodeSymbol::Command::CursorBack:
            //             int display_shift = buffer.MoveCursorBackward();
            //             break;

            //         // case UnicodeSymbol::Command::CursorPosition:
            //         //     inputStartRow = event.GetCSIParameter(0);
            //         //     inputStartColumn = event.GetCSIParameter(1);
            //         //     break;

            //         default:
            //             event.DebugWriteTo(stdout);
            //             break;
            //     }
            // }

            // // if (symbol != -1)
            // //     printf("%d\n", symbol);
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