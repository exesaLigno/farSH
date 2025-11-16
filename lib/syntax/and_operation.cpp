#include "syntax/and_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void AndOperation::DumpNodeTo(FILE* fd) const
{
    fprintf(fd, "\tnode_%x [shape=record; label=And]\n", this);
}

void AndOperation::DumpEdgesTo(FILE* fd) const
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x [label=%s]\n", this, children[idx], children[idx] == First() ? "First" : "Second");
}

AndOperation::AndOperation(Operation* first, Operation* second) : Operation(OperationKind::And)
{
    AppendChild(first);
    AppendChild(second);
}

const Operation* AndOperation::First() const
{
    return children[0];
}

const Operation* AndOperation::Second() const
{
    return children[1];
}
