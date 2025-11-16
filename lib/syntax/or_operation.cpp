#include "syntax/or_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void OrOperation::DumpNodeTo(FILE* fd) const
{
    fprintf(fd, "\tnode_%x [shape=record; label=Or]\n", this);
}

void OrOperation::DumpEdgesTo(FILE* fd) const
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x [label=%s]\n", this, children[idx], children[idx] == First() ? "First" : "Second");
}

OrOperation::OrOperation(Operation* first, Operation* second) : Operation(OperationKind::Or)
{
    AppendChild(first);
    AppendChild(second);
}

const Operation* OrOperation::First() const
{
    return children[0];
}

const Operation* OrOperation::Second() const
{
    return children[1];
}
