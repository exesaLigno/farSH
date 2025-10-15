#include "syntax/raw_string_literal_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void RawStringLiteralOperation::DumpNodeTo(FILE* fd)
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"{ Raw string literal | %s }\"]\n", this, text);
}

void RawStringLiteralOperation::DumpEdgesTo(FILE* fd)
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x\n", this, children[idx]);
}

RawStringLiteralOperation::RawStringLiteralOperation(const char* _text) : Operation(OperationKind::RawStringLiteral)
{
    text = _text;
}
