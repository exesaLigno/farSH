#pragma once

#include <termios.h>
#include <cstdlib>
#include <cstdint>
#include <cstdio>

#include "unicode_symbol.hpp"
#include "unicode_buffer.hpp"

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

    bool FetchSize();
    
    static void SignalHandler(int signo);

public:
    TTY();

    ~TTY();

    UnicodeSymbol Fetch() const;

    void MoveCursor(int32_t rows_shift, int32_t columns_shift) const;
    
    UnicodeBuffer& OutputBuffer();
    
    void Clear();
    
    void Render(bool no_cursor = false);
    
    void Rerender();
};
