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
    {
        fprintf(fd, "\tnode_%x -> node_%x [label=Operand]\n", this, children[idx]);

        if (idx > 0)
            fprintf(fd, "\tnode_%x -> node_%x [label=Redirection, style=dotted]\n", children[idx-1], children[idx]);
    }
}

PipeRedirectionOperation::PipeRedirectionOperation() : Operation(OperationKind::PipeRedirection) { }

PipeRedirectionOperation::PipeRedirectionOperation(std::initializer_list<Operation*> operands) : Operation(OperationKind::PipeRedirection)
{
    for (auto operand : operands)
        AppendChild(operand);
}

void PipeRedirectionOperation::AddOperand(Operation* operand)
{
    AppendChild(operand);
}

int PipeRedirectionOperation::OperandsCount() const
{
    return ChildrenCount();
}

const Operation* PipeRedirectionOperation::GetOperand(int idx) const
{
    return children[idx];
}
