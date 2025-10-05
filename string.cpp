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

    enum class ReallocationMethod
    {
        MultiplySize,
        AppendSize
    };

    /**
     * @brief Reallocates internal buffer with provided method and copies all 
     * information into new allocated buffer.
     * @param value If method `MultiplySize` is selected, size will be multiplied 
     * by provided `value`, if `AppendSize` is selected, size will be increased
     * by `value`.
     * @param method Reallocation method.
     */
    void ReallocateBuffer(size_t value, UnicodeString::ReallocationMethod method)
    {
        assert(
            (method == ReallocationMethod::MultiplySize and value > 1) or 
            (method == ReallocationMethod::AppendSize and value > 0)
        );

        size_t old_buffer_size = bufferSize;
        UnicodeSymbol* old_buffer = buffer;
        
        switch (method)
        {
            case ReallocationMethod::AppendSize:
                bufferSize += value;
                break;
            case ReallocationMethod::MultiplySize:
                bufferSize *= value;
                break;
        }

        buffer = new UnicodeSymbol[bufferSize];

        if (old_buffer)
        {
            memcpy(buffer, old_buffer, old_buffer_size * sizeof(UnicodeSymbol));
            delete[] old_buffer;
        }
    }

    inline UnicodeSymbol& GetFirstEmptySymbolLink()
    {
        return buffer[length];
    }

    inline UnicodeSymbol& ReallocateAndGetFirstEmptySymbolLink()
    {
        if (length >= bufferSize)
            ReallocateBuffer(2, ReallocationMethod::MultiplySize);

        return GetFirstEmptySymbolLink();
    }

public:
    UnicodeString()
    {
        ReallocateBuffer(initialBufferSize, ReallocationMethod::AppendSize);
    }

    UnicodeString(const char* string)
    {
        // Axiom (1).
        // Due to fact that `UnicodeSymbol` consists of at least one byte, it is 
        // general truth that `strlen(string)` is always greater or equal length 
        // of that string converted to `UnicodeSymbol`'s. We can just allocate 
        // space and be sure that it's enough to store provided `string`
        ReallocateBuffer(strlen(string), ReallocationMethod::AppendSize);

        const Byte* string_ptr = (const Byte*) string;

        while (*string_ptr != 0)
        {
            UnicodeSymbol& current = GetFirstEmptySymbolLink();
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

    /** 
     * @brief Appends provided unicode string to the end of current string. If 
     * remaining capacity is less then provided string, current string will
     * be reallocated.
     * @param string Unicode string to be appended to the end of current string.
     */
    void Append(const UnicodeString& string)
    {
        if (length + string.length > bufferSize)
            ReallocateBuffer(string.length, ReallocationMethod::AppendSize);

        for (size_t idx = 0; idx < string.length; idx++)
        {
            // We already allocated enought room for appending string. It is 
            // safe to copy it without reallocations.
            UnicodeSymbol& current = GetFirstEmptySymbolLink();
            current = string.buffer[idx];
            length++;
            width += current.DisplayWidth();
        }
    }

    /** 
     * @brief Inserts provided unicode string in the position `where`. On 
     * insertions into middle of string, remaining part will be moved to make 
     * room for inserting string. If remaining capacity is less then provided 
     * string, current string will be reallocated.
     * @param where Index of element, where to start insertion. Remaining part
     * of string (after index `where`) will be moved forward to make room for 
     * insertion.
     * @param string Unicode string to be inserted into current string on 
     * specified position.
     */
    void Insert(int32_t where, const UnicodeString& string)
    {
        where = (where >= 0) ? where : length + where;
        assert(where <= length);

        if (length + string.length > bufferSize)
            ReallocateBuffer(string.length, ReallocationMethod::AppendSize);

        for (int32_t idx = length - 1; idx >= where; idx--)
            buffer[idx + string.length] = std::move(buffer[idx]);

        for (int32_t idx = 0; idx < string.length; idx++)
            buffer[where + idx] = string.buffer[idx];

        length += string.length;
        width += string.width;
    }

    /**
     * @brief Prepends provided unicode symbol at the beggining of current 
     * string. It reallocates current string internal buffer if it is necessary
     * and moves all contents forward by the length of prepending string.
     * @param string A unicode string to prepend.
     */
    void Prepend(const UnicodeString& string)
    {
        Insert(0, string);
    }

    /** 
     * @brief Appends provided unicode symbol to the end of current string. If 
     * remaining capacity is zero, current string will be reallocated.
     * @param symbol Symbol to be appended to the end of current string.
     */
    void Append(const UnicodeSymbol& symbol)
    {
        UnicodeSymbol& current = ReallocateAndGetFirstEmptySymbolLink();
        current.CloneFrom(symbol);
        length++;
        width += current.DisplayWidth();
    }

    /**
     * @brief Inserts provided symbol at specified position. Moves all data from
     * index `where` to the end of string by one symbol and reallocates if it 
     * necessary
     * @param where Index of element, where to insert new symbol.
     * @param symbol A unicode symbol to insert into string
     */
    void Insert(int32_t where, const UnicodeSymbol& symbol)
    {
        where = (where >= 0) ? where : length + where;
        assert(where <= length);

        if (length + 1 > bufferSize)
            ReallocateBuffer(2, ReallocationMethod::MultiplySize);

        for (int32_t idx = length - 1; idx >= where; idx--)
            buffer[idx + 1] = std::move(buffer[idx]);

        buffer[where].CloneFrom(symbol);

        length++;
        width += symbol.DisplayWidth();
    }

    /**
     * @brief Prepends symbol at the beggining of current string. Moves all 
     * symbols one symbol forward and reallocates buffer if it is needed.
     * @param symbol Unicode symbol to prepend.
     */
    void Prepend(const UnicodeSymbol& symbol)
    {
        Insert(0, symbol);
    }

    void Append(const char* string)
    {
        size_t string_len = strlen(string);
        if (length + string_len > bufferSize)
            ReallocateBuffer(string_len, ReallocationMethod::AppendSize);

        const Byte* string_ptr = (const Byte*) string;
        while (*string_ptr != 0)
        {
            // We already allocated enought room for appending string (Axiom 1). 
            // It is safe to copy it without reallocations.
            UnicodeSymbol& current = GetFirstEmptySymbolLink();
            UnicodeSymbol::CreateInplace(current, string_ptr);
            length++;
            width += current.DisplayWidth();
        }
    }

    void Append(const char symbol)
    {
        UnicodeSymbol& current = ReallocateAndGetFirstEmptySymbolLink();
        current.CloneFrom(UnicodeSymbol(symbol));
        length++;
        width += current.DisplayWidth();
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

    size_t Width(size_t terminal_width = 0) const
    {
        if (terminal_width == 0)
            return width;

        size_t calculated_width = 0;
        for (size_t idx = 0; idx < length; idx++)
        {
            if (buffer[idx].IsCommand() and buffer[idx].GetCommand() == UnicodeSymbol::Command::LineFeed and terminal_width)
                calculated_width += (terminal_width - width % terminal_width);

            else
                calculated_width += buffer[idx].DisplayWidth();
        }

        return calculated_width;
    }
};
