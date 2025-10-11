#include "syntax/raw_string_literal_syntax.hpp"

#include <cstring>

RawStringLiteralSyntax::RawStringLiteralSyntax(const char* _value) : SyntaxNode(SyntaxKind::RawStringLiteral) 
{
    value = new char[strlen(_value) + 1] { 0 };
    strcpy(value, _value);
}

RawStringLiteralSyntax::RawStringLiteralSyntax(char* _value) : SyntaxNode(SyntaxKind::RawStringLiteral) 
{
    value = _value;
}

RawStringLiteralSyntax::~RawStringLiteralSyntax()
{
    if (value)
    {
        delete[] value;
        value = nullptr;
    }

    SyntaxNode::~SyntaxNode();
}

void RawStringLiteralSyntax::DumpNodeTo(FILE* fd)
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"{RawStringLiteralSyntax | %s}\"]\n", this, value);
}
