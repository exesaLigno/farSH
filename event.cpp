#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "symbol.cpp"

class Event
{
public:
    enum class Type
    {
        None,
        WindowResize,
        Input
    };

private:
    Type type = Type::None;
    UnicodeSymbol symbol;

public:
    Event(Type _type): type(_type) { }
    Event(const Byte _bytes[], size_t _bytes_length)
    {
        type = Type::Input;
        symbol = UnicodeSymbol(_bytes, _bytes_length);
    }

    bool Is(Type _type)
    {
        return type == _type;
    }

    bool IsCommand()
    {
        UnicodeSymbol::Type sym_type = symbol.GetType();
        return (
            sym_type == UnicodeSymbol::Type::ControlCode or 
            sym_type == UnicodeSymbol::Type::EscapedSequence
        );
    }

    UnicodeSymbol GetSymbol()
    {
        if (type == Type::Input)
            return symbol;
        else
            return UnicodeSymbol(UnicodeSymbol::Type::Invalid);
    }

    UnicodeSymbol::Type GetSymbolType()
    {
        if (type == Type::Input)
            return symbol.GetType();
        else
            return UnicodeSymbol::Type::Invalid;
    }

    UnicodeSymbol::Command GetCommand()
    {
        if (type == Type::Input)
            return symbol.GetCommand();
        else
            return UnicodeSymbol::Command::None;
    }

    int GetCSIParameter(int position)
    {
        if (type == Type::Input)
            return symbol.GetCSIParameter(position);
        else
            return 0;
    }
};
