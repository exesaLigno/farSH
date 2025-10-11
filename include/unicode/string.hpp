#pragma once

#include <cstdlib>

#include "symbol.hpp"

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
    void ReallocateBuffer(size_t value, UnicodeString::ReallocationMethod method);
    
    int64_t GetAbsoluteIndex(int64_t relative_index) const;
    
    UnicodeString(const UnicodeString& other);

public:
    UnicodeString();

    explicit UnicodeString(const char* string, size_t size = 0);
    
    UnicodeString(UnicodeString&& other);

    ~UnicodeString();

    void Clear();

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
    size_t Insert(int64_t where, const UnicodeString& string);
    
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
    size_t Insert(int64_t where, const char* string, size_t size = 0);

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
    size_t Insert(int64_t where, const UnicodeSymbol& symbol);
    
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
    size_t Insert(int64_t where, const char symbol);
    
    /** 
     * @brief Appends provided unicode string to the end of current string. If 
     * remaining capacity is less then provided string, current string will
     * be reallocated.
     * @param string Unicode string to be appended to the end of current string.
     * @return Count of symbols inserted.
     */
    size_t Append(const UnicodeString& string);
    
    /**
     * @brief Appends provided string to the end of current string. If 
     * remaining capacity is less then provided string, current string will
     * be reallocated.
     * @param string Srting to be inserted.
     * @param size Size of source string. If it equals `0`, or greater then 
     * `strlen(string)`, entire string will be copied.
     * @return Count of symbols inserted.
     */
    size_t Append(const char* string, size_t size = 0);
    
    /** 
     * @brief Appends provided unicode symbol to the end of current string. If 
     * remaining capacity is zero, current string will be reallocated.
     * @param symbol Symbol to be appended to the end of current string.
     * @return Count of symbols inserted.
     */
    size_t Append(const UnicodeSymbol& symbol);
    
    /** 
     * @brief Appends provided symbol to the end of current string. If 
     * remaining capacity is zero, current string will be reallocated.
     * @param symbol Symbol to be appended to the end of current string.
     * @return Count of symbols inserted.
     */
    size_t Append(const char symbol);
    
    /**
     * @brief Prepends provided unicode string at the beggining of current 
     * string. It reallocates current string internal buffer if it is necessary
     * and moves all contents forward by the length of prepending string.
     * @param string A unicode string to prepend.
     * @return Count of symbols inserted.
     */
    size_t Prepend(const UnicodeString& string);
    
    /**
     * @brief Prepends provided string at the beggining of current 
     * string. It reallocates current string internal buffer if it is necessary
     * and moves all contents forward by the length of prepending string.
     * @param string A string to prepend.
     * @param size Size of source string. If it equals `0`, or greater then 
     * `strlen(string)`, entire string will be copied.
     * @return Count of symbols inserted.
     */
    size_t Prepend(const char* string, size_t size = 0);

    /**
     * @brief Prepends symbol at the beggining of current string. Moves all 
     * symbols one symbol forward and reallocates buffer if it is needed.
     * @param symbol Unicode symbol to prepend.
     * @return Count of symbols inserted.
     */
    size_t Prepend(const UnicodeSymbol& symbol);
    
    /**
     * @brief Prepends symbol at the beggining of current string. Moves all 
     * symbols one symbol forward and reallocates buffer if it is needed.
     * @param symbol Symbol to prepend.
     * @return Count of symbols inserted.
     */
    size_t Prepend(const char symbol);
    
    /**
     * @brief Erases `size` symbols in string, starting from position `where`.
     * After erase moves tails to position `where`.
     * @param where First erasing element.
     * @param size Count of symbols to erase. String will be erased till the end
     * if `size` is greater or equal then count of symbols after position 
     * `where`
     */
    void Erase(const size_t where, const size_t size = 1);
    
    UnicodeString operator+(const UnicodeString& other) const;

    UnicodeSymbol& operator[](size_t idx) const;

    void WriteTo(FILE* fd = stdout) const;

    size_t Length() const;
    size_t Width() const;

    UnicodeSymbol* begin() const;
    UnicodeSymbol* end() const;

    friend bool operator==(const UnicodeString& first, const UnicodeString& second);
    friend bool operator==(const UnicodeString& first, const char* second);
};
