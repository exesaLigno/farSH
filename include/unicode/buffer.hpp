#pragma once

#include <cstdlib>

#include "symbol.hpp"
#include "string.hpp"

class UnicodeBuffer : UnicodeString
{
private:
    size_t cursorPosition = 0;
    bool edited = false;

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
        PreserveOriginal, /// Cursor remains in its initial place relative to the text.
        UseAppending, /// Cursor moves to cursor position of appending buffer.
        MoveStart, /// After append cursor moves to the beginning of resulting bufffer.
        MoveEnd, /// After append cursor moves to the end of resulting bufffer.
        MoveJoinPosition /// After append cursor moves to the point between concatenated buffers.
    };

    size_t Append(const UnicodeBuffer& buffer, const CursorMergePolicy merge_policy = CursorMergePolicy::PreserveOriginal);
    size_t Prepend(const UnicodeBuffer& buffer, const CursorMergePolicy merge_policy = CursorMergePolicy::PreserveOriginal);

    void Clear();

    using UnicodeString::Length;
    size_t Width(size_t terminal_width = 0) const;
    size_t WidthUntilCursor(size_t terminal_width = 0) const;
    bool Edited() const;
    void AssumeEdited();

    using UnicodeString::operator[];
    using UnicodeString::WriteTo;
    using UnicodeString::begin;
    using UnicodeString::end;

    friend bool operator==(const UnicodeBuffer& first, const UnicodeBuffer& second);
    friend bool operator==(const UnicodeBuffer& first, const UnicodeString& second);
    friend bool operator==(const UnicodeBuffer& first, const char* second);

    const UnicodeString& AsUnicodeString() const;
};
