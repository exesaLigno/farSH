#include "syntax/argument_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void ArgumentOperation::DumpNodeTo(FILE* fd)
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"Argument\"]\n", this);
}

void ArgumentOperation::DumpEdgesTo(FILE* fd)
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x [label=Value]\n", this, children[idx]);
}

ArgumentOperation::ArgumentOperation() : Operation(OperationKind::Argument)
{

}

const Operation* ArgumentOperation::Value() const
{
    return children[0];
}
