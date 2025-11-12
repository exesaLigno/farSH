#include "syntax/environment_variable_load_operation.hpp"

#include "syntax/operation_kind.hpp"
#include "syntax/operation.hpp"

void EnvironmentVariableLoadOperation::DumpNodeTo(FILE* fd) const
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"EnvironmentVariableReference\"]\n", this);
}

void EnvironmentVariableLoadOperation::DumpEdgesTo(FILE* fd) const
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x [label=\"VariableName\"]\n", this, children[idx]);
}

EnvironmentVariableLoadOperation::EnvironmentVariableLoadOperation(Operation* variable_name) : Operation(OperationKind::EnvironmentVariableReference)
{
    AppendChild(variable_name);
}

const Operation* EnvironmentVariableLoadOperation::VariableName() const
{
    return children[0];
}
