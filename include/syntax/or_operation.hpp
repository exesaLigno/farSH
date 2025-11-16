#pragma once

#include <cstdint>

#include "operation_kind.hpp"
#include "operation.hpp"

class OrOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) const override;
    void DumpEdgesTo(FILE* fd) const override;

public:
    OrOperation(Operation* first, Operation* second);

    const Operation* First() const;
    const Operation* Second() const;
};
