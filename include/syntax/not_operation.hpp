#pragma once

#include <cstdint>

#include "operation_kind.hpp"
#include "operation.hpp"

class NotOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) const override;
    void DumpEdgesTo(FILE* fd) const override;

public:
    NotOperation(Operation* operand);

    const Operation* Operand() const;
};
