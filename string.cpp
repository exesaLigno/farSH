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
    
    int64_t GetAbsoluteIndex(int64_t relative_index)
    {
        int64_t absolute_index = (relative_index >= 0) ? relative_index : length + relative_index;
        assert(absolute_index >= 0);
        assert(absolute_index <= length);
        
        return absolute_index;
    }
    
    UnicodeString(const UnicodeString& other)
    {
        bufferSize = other.bufferSize;
        length = other.length;
        width = other.width;
        
        if (other.buffer and bufferSize > 0)
        {
            buffer = new UnicodeSymbol[bufferSize];
            memcpy(buffer, other.buffer, bufferSize * sizeof(UnicodeSymbol));
        }
    }

public:
    UnicodeString()
    {
        ReallocateBuffer(initialBufferSize, ReallocationMethod::AppendSize);
    }

    explicit UnicodeString(const char* string, size_t size = 0)
    {
        // Axiom (1).
        // Due to fact that `UnicodeSymbol` consists of at least one byte, it is 
        // general truth that `strlen(string)` is always greater or equal length 
        // of that string converted to `UnicodeSymbol`'s. We can just allocate 
        // space and be sure that it's enough to store provided `string`
        size_t string_len = size ? strnlen(string, size) : strlen(string);
        
        ReallocateBuffer(string_len, ReallocationMethod::AppendSize);
        
        for (size_t idx = 0; idx < string_len;)
        {
            idx += UnicodeSymbol::Create(buffer[length], (const Byte*) (string + idx));
            width += buffer[length].DisplayWidth();
            length++;
        }
    }
    
    UnicodeString(UnicodeString&& other)
    {
        bufferSize = other.bufferSize;
        length = other.length;
        width = other.width;
        buffer = other.buffer;
        
        other.bufferSize = 0;
        other.length = 0;
        other.width = 0;
        other.buffer = nullptr;
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
     * @brief Inserts provided unicode string in the position `where`. On 
     * insertions into middle of string, remaining part will be moved to make 
     * room for inserting string. If remaining capacity is less then provided 
     * string, current string will be reallocated.
     * @param where Index of element, where to start insertion. Remaining part
     * of string (after index `where`) will be moved forward to make room for 
     * insertion. If `where` is less then `0`, insertion will start in position
     * `Length() + where`.
     * @param string Unicode string to be inserted into current string on 
     * specified position.
     * @return Count of symbols inserted.
     */
    size_t Insert(int64_t where, const UnicodeString& string)
    {
        int64_t absolute_index = GetAbsoluteIndex(where);

        if (length + string.length > bufferSize)
            ReallocateBuffer(string.length, ReallocationMethod::AppendSize);

        for (int64_t idx = length - 1; idx >= absolute_index; idx--)
            buffer[idx + string.length] = std::move(buffer[idx]);

        for (int64_t idx = 0; idx < string.length; idx++)
            buffer[absolute_index + idx] = string.buffer[idx];

        length += string.length;
        width += string.width;
        
        return string.length;
    }
    
    /**
     * @brief Inserts provided string in the position `where`. On 
     * insertions into middle of string, remaining part will be moved to make 
     * room for inserting string. If remaining capacity is less then provided 
     * string, current string will be reallocated.
     * @param where Index of element, where to start insertion. Remaining part
     * of string (after index `where`) will be moved forward to make room for 
     * insertion. If `where` is less then `0`, insertion will start in position
     * `Length() + where`.
     * @param string Srting to be inserted.
     * @param size Size of source string. If it equals `0`, or greater then 
     * `strlen(string)`, entire string will be copied.
     * @return Count of symbols inserted.
     */
    size_t Insert(int64_t where, const char* string, size_t size = 0)
    {
        size_t string_len = size ? strnlen(string, size) : strlen(string);
        int64_t absolute_index = GetAbsoluteIndex(where);
                
        if (length + string_len > bufferSize)
            ReallocateBuffer(string_len, ReallocationMethod::AppendSize);
        
        for (int64_t idx = length - 1; idx >= absolute_index; idx--)
            buffer[idx + string_len] = std::move(buffer[idx]);
            
        size_t moved_size = length - absolute_index;
        
        size_t inserted_len = 0;
        for (size_t string_idx = 0; string_idx < string_len; inserted_len++)
        {
            string_idx += UnicodeSymbol::Create(buffer[inserted_len + absolute_index], (const Byte*) (string + string_idx));
            width += buffer[inserted_len + absolute_index].DisplayWidth();
            length++;
        }
        
        if (string_len - inserted_len == 0 or moved_size == 0)
            return inserted_len;

        for (size_t idx = 0; idx < moved_size; idx++)
            buffer[absolute_index + inserted_len + idx] = std::move(buffer[absolute_index + string_len + idx]);
            
        return inserted_len;
    }

    /**
     * @brief Inserts provided symbol at specified position. Moves all data from
     * index `where` to the end of string by one symbol and reallocates if it 
     * necessary
     * @param where Index of element, where to start insertion. Remaining part
     * of string (after index `where`) will be moved forward to make room for 
     * insertion. If `where` is less then `0`, insertion will start in position
     * `Length() + where`.
     * @param symbol A unicode symbol to insert into string
     * @return Count of symbols inserted.
     */
    size_t Insert(int64_t where, const UnicodeSymbol& symbol)
    {
        int64_t absolute_index = GetAbsoluteIndex(where);

        if (length + 1 > bufferSize)
            ReallocateBuffer(2, ReallocationMethod::MultiplySize);

        for (int64_t idx = length - 1; idx >= absolute_index; idx--)
            buffer[idx + 1] = std::move(buffer[idx]);

        buffer[absolute_index] = symbol;
        width += buffer[absolute_index].DisplayWidth();
        length++;
        
        return 1;
    }
    
    /**
     * @brief Inserts provided symbol at specified position. Moves all data from
     * index `where` to the end of string by one symbol and reallocates if it 
     * necessary
     * @param where Index of element, where to start insertion. Remaining part
     * of string (after index `where`) will be moved forward to make room for 
     * insertion. If `where` is less then `0`, insertion will start in position
     * `Length() + where`.
     * @param symbol A symbol to insert into string
     * @return Count of symbols inserted.
     */
    size_t Insert(int64_t where, const char symbol)
    {
        return Insert(where, UnicodeSymbol(symbol));
    }
    
    /** 
     * @brief Appends provided unicode string to the end of current string. If 
     * remaining capacity is less then provided string, current string will
     * be reallocated.
     * @param string Unicode string to be appended to the end of current string.
     * @return Count of symbols inserted.
     */
    size_t Append(const UnicodeString& string)
    {
        return Insert(Length(), string);
    }
    
    /**
     * @brief Appends provided string to the end of current string. If 
     * remaining capacity is less then provided string, current string will
     * be reallocated.
     * @param string Srting to be inserted.
     * @param size Size of source string. If it equals `0`, or greater then 
     * `strlen(string)`, entire string will be copied.
     * @return Count of symbols inserted.
     */
    size_t Append(const char* string, size_t size = 0)
    {
        return Insert(Length(), string, size);
    }
    
    /** 
     * @brief Appends provided unicode symbol to the end of current string. If 
     * remaining capacity is zero, current string will be reallocated.
     * @param symbol Symbol to be appended to the end of current string.
     * @return Count of symbols inserted.
     */
    size_t Append(const UnicodeSymbol& symbol)
    {
        return Insert(Length(), symbol);
    }
    
    /** 
     * @brief Appends provided symbol to the end of current string. If 
     * remaining capacity is zero, current string will be reallocated.
     * @param symbol Symbol to be appended to the end of current string.
     * @return Count of symbols inserted.
     */
    size_t Append(const char symbol)
    {
        return Insert(Length(), symbol);
    }
    
    /**
     * @brief Prepends provided unicode string at the beggining of current 
     * string. It reallocates current string internal buffer if it is necessary
     * and moves all contents forward by the length of prepending string.
     * @param string A unicode string to prepend.
     * @return Count of symbols inserted.
     */
    size_t Prepend(const UnicodeString& string)
    {
        return Insert(0, string);
    }
    
    /**
     * @brief Prepends provided string at the beggining of current 
     * string. It reallocates current string internal buffer if it is necessary
     * and moves all contents forward by the length of prepending string.
     * @param string A string to prepend.
     * @param size Size of source string. If it equals `0`, or greater then 
     * `strlen(string)`, entire string will be copied.
     * @return Count of symbols inserted.
     */
    size_t Prepend(const char* string, size_t size = 0)
    {
        return Insert(0, string, size);
    }

    /**
     * @brief Prepends symbol at the beggining of current string. Moves all 
     * symbols one symbol forward and reallocates buffer if it is needed.
     * @param symbol Unicode symbol to prepend.
     * @return Count of symbols inserted.
     */
    size_t Prepend(const UnicodeSymbol& symbol)
    {
        return Insert(0, symbol);
    }
    
    /**
     * @brief Prepends symbol at the beggining of current string. Moves all 
     * symbols one symbol forward and reallocates buffer if it is needed.
     * @param symbol Symbol to prepend.
     * @return Count of symbols inserted.
     */
    size_t Prepend(const char symbol)
    {
        return Insert(0, symbol);
    }
    
    /**
     * @brief Erases `size` symbols in string, starting from position `where`.
     * After erase moves tails to position `where`.
     * @param where First erasing element.
     * @param size Count of symbols to erase. String will be erased till the end
     * if `size` is greater or equal then count of symbols after position 
     * `where`
     */
    void Erase(const size_t where, const size_t size = 1)
    {
        size_t old_length = length;
        
        for (size_t idx = where; idx < where + size and idx < old_length; idx++, length--)
            width -= buffer[idx].DisplayWidth();
            
        for (size_t idx = where + size; idx < old_length; idx++)
            buffer[idx - size] = std::move(buffer[idx]);
    }
    
    UnicodeString operator+(const UnicodeString& other)
    {
        UnicodeString result(*this);
        result.Append(other);
        return result;
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

        // size_t calculated_width = 0;
        // for (size_t idx = 0; idx < length; idx++)
        // {
        //     if (buffer[idx].IsCommand() and buffer[idx].GetCommand() == UnicodeSymbol::Command::LineFeed and terminal_width)
        //         calculated_width += (terminal_width - width % terminal_width);

        //     else
        //         calculated_width += buffer[idx].DisplayWidth();
        // }

        // return calculated_width;
    }
};
