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
        fprintf(fd, "\tnode_%x -> node_%x\n", this, children[idx]);
}

InvocationOperation::InvocationOperation() : Operation(OperationKind::Invocation)
{

}
