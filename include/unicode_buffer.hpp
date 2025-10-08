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
    
    using UnicodeString::Length;
    using UnicodeString::operator[];
    using UnicodeString::WriteTo;
    
    /// TODO: Rewrite to preserve cursorPosition on appending and prepending something
    using UnicodeString::Append;
    using UnicodeString::Prepend;
};
