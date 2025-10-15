#include "syntax/composition_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void CompositionOperation::DumpNodeTo(FILE* fd)
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"Composition\"]\n", this);
}

void CompositionOperation::DumpEdgesTo(FILE* fd)
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x\n", this, children[idx]);
}

CompositionOperation::CompositionOperation() : Operation(OperationKind::Composition)
{

}
