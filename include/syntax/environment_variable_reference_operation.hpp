#pragma once

#include "operation_kind.hpp"
#include "operation.hpp"

class EnvironmentVariableReferenceOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) override;
    void DumpEdgesTo(FILE* fd) override;

public:
    EnvironmentVariableReferenceOperation();

    const Operation* VariableName() const;
};
