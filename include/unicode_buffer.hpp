#pragma once

#include <cstdlib>

#include "unicode_symbol.hpp"
#include "unicode_string.hpp"

class UnicodeBuffer : UnicodeString
{
private:
    size_t cursorPosition = 0;
    
public:
    void MoveCursorForward();
    
    void MoveCursorBackward();
    
    void MoveCursorToStart();

    void MoveCursorToEnd();

    void ClearSymbolBefore();
    
    void ClearSymbolAfter();
    
    void Insert(const UnicodeSymbol& symbol);
    
    void Insert(const char symbol);
    
    void Insert(const UnicodeString& string);
    
    void Insert(const char* string, size_t size = 0);
    
    void SetCursor();
    
    bool CursorAtTheEnd() const;
    
    void Clear();
    
    size_t CursorPosition() const;
    
    size_t WidthUntilPosition(size_t position, size_t terminal_width = 0) const;
    
    size_t Width(size_t terminal_width = 0) const;
    
    size_t WidthUntilCursor(size_t terminal_width = 0) const;
    
    size_t Prepend(const UnicodeString& string);
    size_t Prepend(const char* string, size_t size = 0);
    size_t Prepend(const UnicodeSymbol& symbol);
    size_t Prepend(const char symbol);

    using UnicodeString::Append;

    enum class CursorMergePolicy
    {
        PreserveOriginal,
        UseAppending,
        MoveStart,
        MoveEnd,
        MoveJoinPosition
    };

    size_t Append(const UnicodeBuffer& buffer, const CursorMergePolicy merge_policy = CursorMergePolicy::PreserveOriginal);
    size_t Prepend(const UnicodeBuffer& buffer, const CursorMergePolicy merge_policy = CursorMergePolicy::PreserveOriginal);

    using UnicodeString::Length;
    using UnicodeString::operator[];
    using UnicodeString::WriteTo;
};
