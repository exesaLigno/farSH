#include "syntax/file_redirection_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void FileRedirectionOperation::DumpNodeTo(FILE* fd)
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"File redirection\"]\n", this);
}

void FileRedirectionOperation::DumpEdgesTo(FILE* fd)
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x\n", this, children[idx]);
}

FileRedirectionOperation::FileRedirectionOperation() : Operation(OperationKind::FileRedirection)
{

}
