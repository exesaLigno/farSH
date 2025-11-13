#include "syntax/word_operation.hpp"

#include <cstring>

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void WordOperation::DumpNodeTo(FILE* fd) const
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"{ Word | \\\"%s\\\" }\"]\n", this, text);
}

void WordOperation::DumpEdgesTo(FILE* fd) const
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x\n", this, children[idx]);
}

WordOperation::WordOperation(const char* _text) : Operation(OperationKind::Word)
{
    text = new char[strlen(_text) + 1] { 0 };
    strcpy(text, _text);
}

const char* WordOperation::GetText() const
{
    return text;
}

WordOperation::~WordOperation()
{
    delete[] text;
}
