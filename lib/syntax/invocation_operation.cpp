#include "syntax/invocation_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void InvocationOperation::DumpNodeTo(FILE* fd)
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"Invocation\"]\n", this);
}

void InvocationOperation::DumpEdgesTo(FILE* fd)
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x [label=%s]\n", this, children[idx], children[idx] == ProgramName() ? "ProgramName" : "Argument");
}

InvocationOperation::InvocationOperation() : Operation(OperationKind::Invocation)
{

}

const Operation* InvocationOperation::ProgramName() const
{
    return children[0];
}

size_t InvocationOperation::ArgumentsCount() const
{
    return children_len - 1;
}

const Operation* InvocationOperation::Argument(size_t idx) const
{
    return idx >= children_len - 1 ? nullptr : children[1 + idx];
}
