#include "syntax/command_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void CommandOperation::DumpNodeTo(FILE* fd)
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"Command\"]\n", this);
}

void CommandOperation::DumpEdgesTo(FILE* fd)
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x [label=\"ExecutionUnit\"]\n", this, children[idx]);
}

CommandOperation::CommandOperation() : Operation(OperationKind::Command)
{

}

const Operation* CommandOperation::ExecutionUnit() const
{
    return children[0];
}
