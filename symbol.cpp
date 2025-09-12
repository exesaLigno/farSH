#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>

typedef uint8_t Byte;

class UnicodeSymbol
{
public:
    enum class Type : uint8_t
    {
        None,
        ASCII, ControlCode,
        EscapedSequence,
        UnicodeSymbol,
        Invalid
    };

    enum class Command
    {
        None,
        Bell, Backspace, Tab, LineFeed, FormFeed, CarriageReturn, Del,

        Escape, Stop, Interrupt,
        ArrowUp, ArrowDown, ArrowRight, ArrowLeft,
        CursorPosition,
    };

private:
    Byte* bytes = nullptr;
    size_t bytes_length = 0;
    Type type = Type::None;

public:
    UnicodeSymbol() { }
    UnicodeSymbol(Type _type) : type(_type) { }

    UnicodeSymbol(const Byte* _bytes, const size_t _bytes_length)
    {
        bytes_length = _bytes_length;
        bytes = new Byte[_bytes_length];
        memcpy(bytes, _bytes, bytes_length * sizeof(Byte));

        Byte first = bytes[0];

        if (first >= 0x20 and first <= 0x7E)
            type = Type::ASCII;
        else if ((first >= 0x00 and first <= 0x1A) or (first >= 0x1C and first <= 0x1F) or first == 0x7F)
            type = Type::ControlCode;
        else if (first == 0x1B)
            type = Type::EscapedSequence;
        else if (first >= 0xC2 and first <= 0xF4)
            type = Type::UnicodeSymbol;
        else
        {
            delete[] bytes;
            bytes = nullptr;
            bytes_length = 0;
            type = Type::Invalid;
        }
    }

    ~UnicodeSymbol()
    {
        if (bytes)
        {
            delete[] bytes;
            bytes = nullptr;
        }
    }

    UnicodeSymbol(const UnicodeSymbol& other) noexcept
    {
        type = other.type;
        bytes_length = other.bytes_length;
        bytes = new Byte[bytes_length];
        memcpy(bytes, other.bytes, bytes_length);
    }

    UnicodeSymbol& operator=(const UnicodeSymbol& other) noexcept
    {
        if (this != &other)
        {
            type = other.type;
            bytes_length = other.bytes_length;
            bytes = new Byte[bytes_length];
            memcpy(bytes, other.bytes, bytes_length);
        }

        return *this;
    }

    UnicodeSymbol(UnicodeSymbol&& other) noexcept
    {
        bytes = other.bytes;
        bytes_length = other.bytes_length;
        type = other.type;

        other.bytes = nullptr;
        other.bytes_length = 0;
        other.type = Type::Invalid;
    }

    UnicodeSymbol& operator=(UnicodeSymbol&& other) noexcept
    {
        if (this != &other)
        {
            delete[] bytes;

            bytes = other.bytes;
            bytes_length = other.bytes_length;
            type = other.type;

            other.bytes = nullptr;
            other.bytes_length = 0;
            other.type = Type::Invalid;
        }

        return *this;
    }

    Type GetType()
    {
        return type;
    }

    void DebugWriteTo(FILE* fileno = stdout)
    {
        char buf[bytes_length + 1];
        memcpy(buf, bytes, bytes_length);
        buf[bytes_length] = 0;

        switch (type)
        {
            case Type::None:
            case Type::Invalid:
                fprintf(fileno, "<INVALID>");
                break;

            case Type::ASCII:
                fprintf(fileno, "<ASCII %c>", buf[0]);
                break;

            case Type::ControlCode:
                fprintf(fileno, "<Control code 0x%02x>", buf[0]);
                break;

            case Type::EscapedSequence:
                fprintf(fileno, "<Escaped sequence (%d bytes) \\e%s>", bytes_length, buf + sizeof(char));
                break;

            case Type::UnicodeSymbol:
                fprintf(fileno, "<Unicode symbol (%d bytes) ", bytes_length);
                for (size_t idx = 0; idx < bytes_length; idx++)
                    fprintf(fileno, "%02x ", bytes[idx]);
                fprintf(fileno, "%s>", buf);
                break;
        }
    }

    void WriteTo(FILE* fileno = stdout) const
    {
        if (type != Type::None and type != Type::Invalid)
            for (size_t idx = 0; idx < bytes_length; idx++)
                fputc(bytes[idx], fileno);
    }

    bool IsCorrect()
    {
        return type != Type::None and type != Type::Invalid;
    }

    Command GetCommand()
    {
        switch (type)
        {
            case Type::ControlCode:
                switch (bytes[0])
                {
                    case 0x07: return Command::Bell;
                    case 0x08: return Command::Backspace;
                    case 0x09: return Command::Tab;
                    case 0x0A: return Command::LineFeed;
                    case 0x0C: return Command::FormFeed;
                    case 0x0D: return Command::CarriageReturn;
                    case 0x7F: return Command::Del;
                    default: return Command::None;
                }

            case Type::EscapedSequence:
                switch (bytes[bytes_length - 1])
                {
                    case 'R': return Command::CursorPosition;
                    case 'A': return Command::ArrowUp;
                    case 'B': return Command::ArrowDown;
                    case 'C': return Command::ArrowRight;
                    case 'D': return Command::ArrowLeft;
                }
        }

        return Command::None;
    }

    int GetCSIParameter(size_t position)
    {
        int result = 0;
        size_t cur_pos = 0;

        for (size_t idx = 0; idx < bytes_length; idx++)
        {
            if (cur_pos == position and bytes[idx] >= 0x30 and bytes[idx] <= 0x39)
                result = 10 * result + (bytes[idx] - 0x30);
            if (bytes[idx] == 0x3B)
                cur_pos++;
        }

        return result;
    }
};