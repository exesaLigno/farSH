#pragma once

#include <cstdint>
#include <cstdio>

#include "symbol.cpp"

class UnicodeBuffer
{
public:
    UnicodeSymbol data[512];
    size_t buffer_capacity = 512;
    size_t buffer_length = 0;
    size_t cursor_position = 0;
    size_t visible_cursor_position = 0;

    int MoveCursorForward(size_t symbols_count = 1)
    {
        size_t old_pos = cursor_position;

        cursor_position += symbols_count;
        if (cursor_position > buffer_length)
            cursor_position = buffer_length;

        return cursor_position - old_pos;
    }

    int MoveCursorBackward(size_t symbols_count = 1)
    {
        size_t old_pos = cursor_position;

        cursor_position -= symbols_count;
        if (cursor_position < 0)
            cursor_position = 0;

        return cursor_position - old_pos;
    }

    int ClearSymbolBefore()
    {
        if (cursor_position == 0)
            return 0;

        for (size_t idx = cursor_position; idx <= buffer_length; idx++)
            data[idx - 1] = data[idx];

        buffer_length--;
        cursor_position--;

        return -1;
    }

    // void ClearWordBefore()
    // {
    // }

    int ClearSymbolAfter()
    {
        for (size_t idx = cursor_position + 1; idx <= buffer_length; idx++)
            data[idx - 1] = data[idx];

        buffer_length--;

        return 0;
    }

    int Insert(UnicodeSymbol symbol)
    {
        if (cursor_position == buffer_length)
        {
            buffer_length++;
            data[cursor_position++] = symbol;
        }

        else
        {
            buffer_length++;

            for (size_t idx = buffer_length; idx > cursor_position; idx--)
                data[idx] = data[idx - 1];

            data[cursor_position++] = symbol;
        }

        return 1;
    }

    void Reset()
    {
        buffer_length = 0;
        cursor_position = 0;
    }

    void WriteTo(FILE* fileno = stdout) const
    {
        for (size_t idx = 0; idx < buffer_length; idx++)
            data[idx].WriteTo(fileno);
    }
};
