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
    size_t current_position = 0;

    int MoveCursorForward(size_t distance = 1)
    {
        size_t old_pos = current_position;

        current_position += distance;
        if (current_position > buffer_length)
            current_position = buffer_length;

        return current_position - old_pos;
    }

    int MoveCursorBackward(size_t distance = 1)
    {
        size_t old_pos = current_position;

        current_position -= distance;
        if (current_position < 0)
            current_position = 0;

        return current_position - old_pos;
    }

    int ClearSymbolBefore()
    {
        if (current_position == 0)
            return 0;

        for (size_t idx = current_position; idx <= buffer_length; idx++)
            data[idx - 1] = data[idx];

        buffer_length--;
        current_position--;

        return -1;
    }

    // void ClearWordBefore()
    // {
    // }

    int ClearSymbolAfter()
    {
        for (size_t idx = current_position + 1; idx <= buffer_length; idx++)
            data[idx - 1] = data[idx];

        buffer_length--;

        return 0;
    }

    int Insert(UnicodeSymbol symbol)
    {
        if (current_position == buffer_length)
        {
            buffer_length++;
            data[current_position++] = symbol;
        }

        else
        {
            buffer_length++;

            for (size_t idx = buffer_length; idx > current_position; idx--)
                data[idx] = data[idx - 1];

            data[current_position++] = symbol;
        }

        return 1;
    }

    void Reset()
    {
        buffer_length = 0;
        current_position = 0;
    }

    void WriteTo(FILE* fileno = stdout) const
    {
        for (size_t idx = 0; idx < buffer_length; idx++)
            data[idx].WriteTo(fileno);
    }
};