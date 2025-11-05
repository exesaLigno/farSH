#include "syntax/file_redirection_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void FileRedirectionOperation::DumpNodeTo(FILE* fd) const
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"File redirection\"]\n", this);
}

void FileRedirectionOperation::DumpEdgesTo(FILE* fd) const
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x [label=%s]\n", this, children[idx], children[idx] == Source() ? "Source" : "Destination");
}

FileRedirectionOperation::FileRedirectionOperation() : Operation(OperationKind::FileRedirection)
{

}

const Operation* FileRedirectionOperation::Source() const
{
    return children[0];
}

const Operation* FileRedirectionOperation::Destination() const
{
    return children[1];
}
