#pragma once

#include "operation_kind.hpp"
#include "operation.hpp"

class CommandOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) const override;
    void DumpEdgesTo(FILE* fd) const override;

public:
    CommandOperation();

    const Operation* ExecutionUnit() const;
};
