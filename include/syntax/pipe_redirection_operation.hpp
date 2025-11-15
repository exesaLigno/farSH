#pragma once

#include "operation_kind.hpp"
#include "operation.hpp"

class PipeRedirectionOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) const override;
    void DumpEdgesTo(FILE* fd) const override;

public:
    PipeRedirectionOperation();
    PipeRedirectionOperation(std::initializer_list<Operation*> operands);

    void AddOperand(Operation* operand);

    int OperandsCount() const;
    const Operation* GetOperand(int idx) const;
};
