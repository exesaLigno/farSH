#include "syntax/pipe_redirection_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void PipeRedirectionOperation::DumpNodeTo(FILE* fd) const
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"Pipe redirection\"]\n", this);
}

void PipeRedirectionOperation::DumpEdgesTo(FILE* fd) const
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x [label=%s]\n", this, children[idx], children[idx] == Source() ? "Source" : "Destination");
}

PipeRedirectionOperation::PipeRedirectionOperation(Operation* source, Operation* destination) : Operation(OperationKind::PipeRedirection)
{
    AppendChild(source);
    AppendChild(destination);
}

const Operation* PipeRedirectionOperation::Source() const
{
    return children[0];
}

const Operation* PipeRedirectionOperation::Destination() const
{
    return children[1];
}
