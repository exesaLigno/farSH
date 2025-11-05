#include "syntax/environment_variable_reference_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void EnvironmentVariableReferenceOperation::DumpNodeTo(FILE* fd) const
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"EnvironmentVariableReference\"]\n", this);
}

void EnvironmentVariableReferenceOperation::DumpEdgesTo(FILE* fd) const
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x [label=\"VariableName\"]\n", this, children[idx]);
}

EnvironmentVariableReferenceOperation::EnvironmentVariableReferenceOperation() : Operation(OperationKind::EnvironmentVariableReference)
{

}

const Operation* EnvironmentVariableReferenceOperation::VariableName() const
{
    return children[0];
}
