#pragma once

#include <cstdint>
#include <cstdio>

#include "symbol.cpp"

class UnicodeBuffer
{
public:
    UnicodeSymbol data[512];
    size_t bufferCapacity = 512;
    size_t bufferLength = 0;
    size_t cursorPosition = 0;
    size_t displayCursorPosition = 0;

    int MoveCursorForward()
    {
        uint8_t display_shift = 0;

        while (display_shift == 0)
        {
            if (cursorPosition == bufferLength)
                return 0;

            display_shift = data[cursorPosition].DisplayWidth();
            cursorPosition++;
            displayCursorPosition += display_shift;
        }

        return display_shift;
    }

    int MoveCursorBackward()
    {
        uint8_t display_shift = 0;

        while (display_shift == 0)
        {
            if (cursorPosition == 0)
                return 0;

            cursorPosition--;
            display_shift = data[cursorPosition].DisplayWidth();
            displayCursorPosition -= display_shift;
        }

        return -display_shift;
    }

    int ClearSymbolBefore()
    {
        uint8_t removed_display_width = 0;

        if (cursorPosition == 0)
            return 0;

        while (removed_display_width == 0)
        {
            removed_display_width = data[cursorPosition - 1].DisplayWidth();

            for (size_t idx = cursorPosition; idx <= bufferLength; idx++)
                data[idx - 1] = data[idx];

            bufferLength--;
            cursorPosition--;
            displayCursorPosition -= removed_display_width;
        }

        return -removed_display_width;
    }

    // void ClearWordBefore()
    // {
    // }

    int ClearSymbolAfter()
    {
        if (cursorPosition < bufferLength)
        {
            for (size_t idx = cursorPosition + 1; idx <= bufferLength; idx++)
                data[idx - 1] = data[idx];

            bufferLength--;
        }

        return 0;
    }

    int Insert(UnicodeSymbol symbol)
    {
        if (cursorPosition == bufferLength)
        {
            bufferLength++;
            data[cursorPosition++] = symbol;
            displayCursorPosition += symbol.DisplayWidth();
        }

        else
        {
            bufferLength++;

            for (size_t idx = bufferLength; idx > cursorPosition; idx--)
                data[idx] = data[idx - 1];

            data[cursorPosition++] = symbol;
            displayCursorPosition += symbol.DisplayWidth();
        }

        return symbol.DisplayWidth();
    }

    void Insert(const char sym)
    {
        Insert(UnicodeSymbol(sym));
    }

    void Insert(const char* str, size_t size = 0)
    {
        size_t idx = 0;
        while ((size == 0 or idx < size) and str[idx] != '\0')
            Insert(UnicodeSymbol::Create([&str, &idx]() {
                return str[idx++];
            }));
    }

    void Reset()
    {
        bufferLength = 0;
        cursorPosition = 0;
        displayCursorPosition = 0;
    }

    void WriteTo(FILE* fileno = stdout) const
    {
        for (size_t idx = 0; idx < bufferLength; idx++)
            data[idx].WriteTo(fileno);
    }

    UnicodeSymbol operator[](size_t idx) const
    {
        return data[idx];
    }
};
