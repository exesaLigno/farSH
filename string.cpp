#pragma once

#include <functional>

#include "symbol.cpp"

class UnicodeString
{
private:
    static constexpr size_t initialBufferSize = 16;

    UnicodeSymbol* buffer = nullptr;
    size_t bufferSize = 0;
    size_t length = 0;
    size_t width = 0;

    void ReallocateBuffer(size_t multiplier = 2, size_t added_size = 0)
    {
        size_t old_buffer_size = bufferSize;
        UnicodeSymbol* old_buffer = buffer;

        bufferSize = bufferSize * multiplier + added_size;
        buffer = new UnicodeSymbol[bufferSize];
        memcpy(buffer, old_buffer, old_buffer_size * sizeof(UnicodeSymbol));
        delete[] old_buffer;
    }

    UnicodeSymbol& ReallocateAndGetFirstEmptySymbolLink()
    {
        if (length >= bufferSize)
            ReallocateBuffer();

        return buffer[length];
    }

    void ReallocateAndInsert(const UnicodeSymbol& symbol)
    {
        if (length >= bufferSize)
            ReallocateBuffer();

        buffer[length++] = symbol;
        width += symbol.DisplayWidth();
    }

public:
    UnicodeString()
    {
        ReallocateBuffer(0, initialBufferSize);
    }

    UnicodeString(const char* string)
    {
        ReallocateBuffer(0, strlen(string));

        const Byte* string_ptr = (const Byte*) string;

        while (*string_ptr != 0)
        {
            UnicodeSymbol& current = ReallocateAndGetFirstEmptySymbolLink();
            UnicodeSymbol::CreateInplace(current, string_ptr);
            length++;
            width += current.DisplayWidth();
        }
    }

    ~UnicodeString()
    {
        if (buffer)
        {
            delete[] buffer;
            buffer = nullptr;
        }
    }

    void Clear()
    {
        length = 0;
        width = 0;
    }

    UnicodeString& operator+=(const UnicodeString& string)
    {
        for (size_t idx = 0; idx < string.length; idx++)
        {
            UnicodeSymbol& current = ReallocateAndGetFirstEmptySymbolLink();
            current = string.buffer[idx];
            length++;
            width += current.DisplayWidth();
        }

        return *this;
    }

    UnicodeString& operator+=(const UnicodeSymbol& symbol)
    {
        ReallocateAndInsert(symbol);
        return *this;
    }

    UnicodeString& operator+=(const char* string)
    {
        const Byte* string_ptr = (const Byte*) string;
        while (*string_ptr != 0)
        {
            UnicodeSymbol& current = ReallocateAndGetFirstEmptySymbolLink();
            UnicodeSymbol::CreateInplace(current, string_ptr);
            length++;
            width += current.DisplayWidth();
        }
        return *this;
    }

    UnicodeString& operator+=(const char symbol)
    {
        ReallocateAndInsert(UnicodeSymbol(symbol));
        return *this;
    }

    UnicodeSymbol& operator[](size_t idx)
    {
        return buffer[idx];
    }

    void WriteTo(FILE* fd = stdout) const
    {
        for (size_t idx = 0; idx < length; idx++)
            buffer[idx].WriteTo(fd);
    }

    size_t Length() const
    {
        return length;
    }

    size_t Width() const
    {
        return width;
    }
};
