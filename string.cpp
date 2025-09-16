#pragma once

#include <functional>

#include "symbol.cpp"

class UnicodeString
{
private:
    size_t cursorPosition = 0;
    size_t size = 0;
    size_t width = 0;

public:
    UnicodeString(const char* string)
    {
        const char* string_ptr = string;
        
        std::function<Byte()> string_stream = [&string_ptr]() {
            return *(string_ptr++);
        };

        while (*string_ptr != '\0')
        {
            UnicodeSymbol symbol = UnicodeSymbol::CreateFromStream(string_stream);
        }
    }

    UnicodeString operator+(const UnicodeString& other) const
    {

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
        size += string.size;
        width += string.width;
    }

    UnicodeString& operator+=(const UnicodeSymbol& symbol)
    {
        size += 1;
        width += symbol.GetGlyphWidth();
    }

    UnicodeString& operator+=(const char* string)
    {

    }

    UnicodeString& operator+=(const char symbol)
    {

    }
};