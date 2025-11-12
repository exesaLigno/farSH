#pragma once

#include "operation_kind.hpp"
#include "operation.hpp"

class EnvironmentVariableLoadOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) const override;
    void DumpEdgesTo(FILE* fd) const override;

public:
    EnvironmentVariableLoadOperation(Operation* variable_name);

    const Operation* VariableName() const;
};
