#include "unicode_buffer.hpp"

#include <cstdlib>

void UnicodeBuffer::MoveCursorForward()
{
    uint8_t display_shift = 0;

    while (display_shift == 0 and cursorPosition < Length())
    {
        display_shift = operator[](cursorPosition).DisplayWidth();
        cursorPosition++;
    }
}

void UnicodeBuffer::MoveCursorBackward()
{
    uint8_t display_shift = 0;

    while (display_shift == 0 and cursorPosition > 0)
    {
        cursorPosition--;
        display_shift = operator[](cursorPosition).DisplayWidth();
    }
}

void UnicodeBuffer::MoveCursorToStart()
{
    cursorPosition = 0;
}

void UnicodeBuffer::MoveCursorToEnd()
{
    cursorPosition = Length();
}

void UnicodeBuffer::ClearSymbolBefore()
{
    size_t old_cursor_position = cursorPosition;

    while (cursorPosition > 0 and operator[](--cursorPosition).DisplayWidth() == 0);
    
    Erase(cursorPosition, old_cursor_position - cursorPosition);
}

void UnicodeBuffer::ClearSymbolAfter()
{
    size_t removing_length = 0;
    
    while (operator[](cursorPosition + removing_length++).DisplayWidth() == 0 and cursorPosition < Length());
    
    Erase(cursorPosition, removing_length);
}

void UnicodeBuffer::Insert(const UnicodeSymbol& symbol)
{
    cursorPosition += UnicodeString::Insert(cursorPosition, symbol);
}

void UnicodeBuffer::Insert(const char symbol)
{
    cursorPosition += UnicodeString::Insert(cursorPosition, symbol);
}

void UnicodeBuffer::Insert(const UnicodeString& string)
{
    cursorPosition += UnicodeString::Insert(cursorPosition, string);
}

void UnicodeBuffer::Insert(const char* string, size_t size)
{
    cursorPosition += UnicodeString::Insert(cursorPosition, string, size);
}

void UnicodeBuffer::SetCursor()
{
    cursorPosition = Length();
}

bool UnicodeBuffer::CursorAtTheEnd() const
{
    return cursorPosition == Length();
}

void UnicodeBuffer::Clear()
{
    UnicodeString::Clear();
    cursorPosition = 0;
}

size_t UnicodeBuffer::CursorPosition() const
{
    return cursorPosition;
}

size_t UnicodeBuffer::WidthUntilPosition(size_t position, size_t terminal_width) const
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

size_t UnicodeBuffer::Width(size_t terminal_width) const
{
    if (terminal_width == 0)
        return UnicodeString::Width();
        
    return WidthUntilPosition(Length(), terminal_width);
}

size_t UnicodeBuffer::WidthUntilCursor(size_t terminal_width) const
{
    return WidthUntilPosition(CursorPosition(), terminal_width);
}
