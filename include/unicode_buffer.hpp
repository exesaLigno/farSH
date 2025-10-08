#pragma once

#include <cstdlib>

#include "unicode_symbol.hpp"
#include "unicode_string.hpp"

class UnicodeBuffer : UnicodeString
{
private:
    size_t cursorPosition = 0;

    size_t Width(size_t from, size_t to, size_t terminal_width = 0) const;

public:
    void MoveCursorForward();
    void MoveCursorBackward();
    void MoveCursorTokenForward();
    void MoveCursorTokenBackward();
    void MoveCursorToStart();
    void MoveCursorToEnd();

    bool CursorAtTheEnd() const;
    size_t CursorPosition() const;

    void ClearSymbolBefore();
    void ClearSymbolAfter();

    void Insert(const UnicodeSymbol& symbol);
    void Insert(const char symbol);
    void Insert(const UnicodeString& string);
    void Insert(const char* string, size_t size = 0);

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

    void Clear();

    using UnicodeString::Length;
    size_t Width(size_t terminal_width = 0) const;
    size_t WidthUntilCursor(size_t terminal_width = 0) const;

    using UnicodeString::operator[];
    using UnicodeString::WriteTo;
};
