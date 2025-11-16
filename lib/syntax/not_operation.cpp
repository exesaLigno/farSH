#include "syntax/not_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void NotOperation::DumpNodeTo(FILE* fd) const
{
    fprintf(fd, "\tnode_%x [shape=record; label=Not]\n", this);
}

void NotOperation::DumpEdgesTo(FILE* fd) const
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x [label=Operand]\n", this, children[idx]);
}

NotOperation::NotOperation(Operation* operand) : Operation(OperationKind::Not)
{
    AppendChild(operand);
}

const Operation* NotOperation::Operand() const
{
    return children[0];
}
