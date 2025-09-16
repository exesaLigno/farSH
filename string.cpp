#pragma once

#include <functional>

#include "symbol.cpp"

class UnicodeString
{
private:
    size_t cursorPosition = 0;
    size_t displayCursorPosition = 0;
    size_t length = 0;
    size_t displayWidth = 0;

public:
    UnicodeString() { }
    UnicodeString(const char* string)
    {
        const char* string_ptr = string;
        
        while (*string_ptr != '\0')
        {
            UnicodeSymbol symbol = UnicodeSymbol::CreateFromStream([&string_ptr]() {
                return *(string_ptr++);
            });
        }
    }

    void Clear()
    {
        cursorPosition = 0;
        displayCursorPosition = 0;
        length = 0;
        displayWidth = 0;
    }

    void SetCursorPosition(size_t new_position)
    {
        
    }

    void SetDisplayCursorPosition(size_t new_position)
    {

    }

    UnicodeString operator+(const UnicodeString& other) const
    {
        UnicodeString result;

        result.length = length + other.length;
        result.displayWidth = displayWidth + other.displayWidth;
    }

    UnicodeString operator+(const UnicodeSymbol& other) const
    {

    }

    UnicodeString operator+(const char* string) const
    {

    }

    UnicodeString operator+(const char symbol) const
    {

    }

    UnicodeString& operator+=(const UnicodeString& string)
    {
        length += string.length;
        displayWidth += string.displayWidth;
    }

    UnicodeString& operator+=(const UnicodeSymbol& symbol)
    {
        length += 1;
        displayWidth += symbol.DisplayWidth();
    }

    UnicodeString& operator+=(const char* string)
    {

    }

    UnicodeString& operator+=(const char symbol)
    {

    }

    size_t DisplayWidth()
    {
        return displayWidth;
    }

    size_t CursorPosition()
    {
        return cursorPosition;
    }

    size_t DisplaCursorPosition()
    {
        return displayCursorPosition;
    }

    void WriteTo(FILE* stream)
    {

    }
};