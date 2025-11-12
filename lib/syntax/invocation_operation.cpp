#include "syntax/invocation_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void InvocationOperation::DumpNodeTo(FILE* fd) const
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"Invocation\"]\n", this);
}

void InvocationOperation::DumpEdgesTo(FILE* fd) const
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x [label=%s]\n", this, children[idx], children[idx] == ProgramName() ? "ProgramName" : "Argument");
}

InvocationOperation::InvocationOperation(Operation* program_name, std::initializer_list<Operation*> arguments) : Operation(OperationKind::Invocation)
{
    AppendChild(program_name);

    for (auto argument : arguments)
        AppendChild(argument);
}

const Operation* InvocationOperation::ProgramName() const
{
    return children[0];
}

size_t InvocationOperation::ArgumentsCount() const
{
    return children_len - 1;
}

const Operation* InvocationOperation::Argument(size_t idx) const
{
    return idx >= children_len - 1 ? nullptr : children[1 + idx];
}
