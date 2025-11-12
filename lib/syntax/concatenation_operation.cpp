#include "syntax/concatenation_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void ConcatenationOperation::DumpNodeTo(FILE* fd) const
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"Concatenation\"]\n", this);
}

void ConcatenationOperation::DumpEdgesTo(FILE* fd) const
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x\n", this, children[idx]);
}

ConcatenationOperation::ConcatenationOperation(std::initializer_list<Operation*> components) : Operation(OperationKind::Concatenation)
{
    for (auto component : components)
        AppendChild(component);
}

size_t ConcatenationOperation::ValuesCount() const
{
    return children_len;
}

const Operation* ConcatenationOperation::Value(size_t idx) const
{
    return idx >= children_len ? nullptr : children[idx];
}
