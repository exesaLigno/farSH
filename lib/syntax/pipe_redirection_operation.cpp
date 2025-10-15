#include "syntax/pipe_redirection_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void PipeRedirectionOperation::DumpNodeTo(FILE* fd)
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"Pipe redirection\"]\n", this);
}

void PipeRedirectionOperation::DumpEdgesTo(FILE* fd)
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x\n", this, children[idx]);
}

PipeRedirectionOperation::PipeRedirectionOperation() : Operation(OperationKind::PipeRedirection)
{

}
