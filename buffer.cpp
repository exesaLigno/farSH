#pragma once

#include <cstdint>
#include <cstdio>

#include "symbol.cpp"
#include "string.cpp"

class UnicodeBuffer : UnicodeString
{
private:
    size_t cursorPosition = 0;
    
public:
    void MoveCursorForward()
    {
        uint8_t display_shift = 0;

        while (display_shift == 0 and cursorPosition < Length())
        {
            display_shift = operator[](cursorPosition).DisplayWidth();
            cursorPosition++;
        }
    }
    
    void MoveCursorBackward()
    {
        uint8_t display_shift = 0;

        while (display_shift == 0 and cursorPosition > 0)
        {
            cursorPosition--;
            display_shift = operator[](cursorPosition).DisplayWidth();
        }
    }
    
    void MoveCursorToStart()
    {
        cursorPosition = 0;
    }

    void MoveCursorToEnd()
    {
        cursorPosition = Length();
    }

    void ClearSymbolBefore()
    {
        size_t old_cursor_position = cursorPosition;

        while (cursorPosition > 0 and operator[](--cursorPosition).DisplayWidth() == 0);
        
        Erase(cursorPosition, old_cursor_position - cursorPosition);
    }
    
    void ClearSymbolAfter()
    {
        size_t removing_length = 0;
        
        while (operator[](cursorPosition + removing_length++).DisplayWidth() == 0 and cursorPosition < Length());
        
        Erase(cursorPosition, removing_length);
    }
    
    void Insert(const UnicodeSymbol& symbol)
    {
        cursorPosition += UnicodeString::Insert(cursorPosition, symbol);
    }
    
    void Insert(const char symbol)
    {
        cursorPosition += UnicodeString::Insert(cursorPosition, symbol);
    }
    
    void Insert(const UnicodeString& string)
    {
        cursorPosition += UnicodeString::Insert(cursorPosition, string);
    }
    
    void Insert(const char* string, size_t size = 0)
    {
        cursorPosition += UnicodeString::Insert(cursorPosition, string, size);
    }
    
    void SetCursor()
    {
        cursorPosition = Length();
    }
    
    bool CursorAtTheEnd() const
    {
        return cursorPosition == Length();
    }
    
    void Clear()
    {
        UnicodeString::Clear();
        cursorPosition = 0;
    }
    
    size_t CursorPosition() const
    {
        return cursorPosition;
    }
    
    size_t WidthUntilPosition(size_t position, size_t terminal_width = 0) const
    {
        size_t calculated_width = 0;
        for (size_t idx = 0; idx < position; idx++)
        {
            if (terminal_width and operator[](idx).IsCommand() and operator[](idx).GetCommand() == UnicodeSymbol::Command::LineFeed)
                calculated_width += (terminal_width - calculated_width % terminal_width);
                
            else if (terminal_width and operator[](idx).DisplayWidth() > terminal_width - calculated_width % terminal_width)
                calculated_width += terminal_width - calculated_width % terminal_width + operator[](idx).DisplayWidth();

            else
                calculated_width += operator[](idx).DisplayWidth();
        }

        return calculated_width;
    }
    
    size_t Width(size_t terminal_width = 0) const
    {
        if (terminal_width == 0)
            return UnicodeString::Width();
            
        return WidthUntilPosition(Length(), terminal_width);
    }
    
    size_t WidthUntilCursor(size_t terminal_width = 0) const
    {
        return WidthUntilPosition(CursorPosition(), terminal_width);
    }
    
    using UnicodeString::Length;
    using UnicodeString::operator[];
    using UnicodeString::WriteTo;
    
    /// TODO: Rewrite to preserve cursorPosition on appending and prepending something
    using UnicodeString::Append;
    using UnicodeString::Prepend;
};
