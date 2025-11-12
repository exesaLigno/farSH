#include "syntax/file_redirection_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void FileRedirectionOperation::DumpNodeTo(FILE* fd) const
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"{File redirection | { Flags | ", this);
    if (HasFlag(F_APPEND))
        fprintf(fd, "F_APPEND ");
    if (HasFlag(F_REDIRECT_STDOUT))
        fprintf(fd, "F_REDIRECT_STDOUT");
    if (HasFlag(F_REDIRECT_STDERR))
        fprintf(fd, "F_REDIRECT_STDERR");
    fprintf(fd, " }}\"]\n");
}

void FileRedirectionOperation::DumpEdgesTo(FILE* fd) const
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x [label=%s]\n", this, children[idx], children[idx] == Source() ? "Source" : "Destination");
}

FileRedirectionOperation::FileRedirectionOperation(Operation* source, Operation* destination) : Operation(OperationKind::FileRedirection)
{
    AppendChild(source);
    AppendChild(destination);

    redirectionFlags |= F_REDIRECT_STDOUT;
    // if (append) redirectionFlags |= F_APPEND;
}

const Operation* FileRedirectionOperation::Source() const
{
    return children[0];
}

const Operation* FileRedirectionOperation::Destination() const
{
    return children[1];
}

bool FileRedirectionOperation::HasFlag(uint8_t flag) const
{
    return (redirectionFlags & flag) == flag;
}
