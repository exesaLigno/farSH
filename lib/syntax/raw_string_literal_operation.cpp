#include "syntax/raw_string_literal_operation.hpp"

#include <cstring>

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void RawStringLiteralOperation::DumpNodeTo(FILE* fd) const
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"{ RawStringLiteral | %s }\"]\n", this, text);
}

void RawStringLiteralOperation::DumpEdgesTo(FILE* fd) const
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x\n", this, children[idx]);
}

RawStringLiteralOperation::RawStringLiteralOperation(const char* _text) : Operation(OperationKind::RawStringLiteral)
{
    text = new char[strlen(_text) + 1] { 0 };
    strcpy(text, _text);
}

const char* RawStringLiteralOperation::GetText() const
{
    return text;
}

RawStringLiteralOperation::~RawStringLiteralOperation()
{
    delete[] text;
}
