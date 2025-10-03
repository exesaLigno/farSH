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

    void AppendSymbolAndReallocate(const UnicodeSymbol& sym)
    {
        if (length >= bufferSize)
            ReallocateBuffer();
        buffer[length++] = sym;
        width += sym.DisplayWidth();
    }

public:
    UnicodeString()
    {
        ReallocateBuffer(0, initialBufferSize);
    }

    UnicodeString(const char* string)
    {
        ReallocateBuffer(0, initialBufferSize);

        const char* string_ptr = string;
        while (*string_ptr != '\0')
        {
            AppendSymbolAndReallocate(UnicodeSymbol::Create([&string_ptr]() {
                return *(string_ptr++);
            }));
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
            AppendSymbolAndReallocate(string.buffer[idx]);

        return *this;
    }

    UnicodeString& operator+=(const UnicodeSymbol& symbol)
    {
        AppendSymbolAndReallocate(symbol);
        return *this;
    }

    UnicodeString& operator+=(const char* string)
    {
        const char* string_ptr = string;
        while (*string_ptr != '\0')
        {
            AppendSymbolAndReallocate(UnicodeSymbol::Create([&string_ptr]() {
                return *(string_ptr++);
            }));
        }
        return *this;
    }

    UnicodeString& operator+=(const char symbol)
    {
        AppendSymbolAndReallocate(UnicodeSymbol(symbol));
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
