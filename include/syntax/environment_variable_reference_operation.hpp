#pragma once

#include "operation_kind.hpp"
#include "operation.hpp"

class EnvironmentVariableReferenceOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) const override;
    void DumpEdgesTo(FILE* fd) const override;

public:
    EnvironmentVariableReferenceOperation();

    const Operation* VariableName() const;
};
