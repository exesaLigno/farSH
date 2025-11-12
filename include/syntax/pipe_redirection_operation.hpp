#pragma once

#include "operation_kind.hpp"
#include "operation.hpp"

class PipeRedirectionOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) const override;
    void DumpEdgesTo(FILE* fd) const override;

public:
    PipeRedirectionOperation(Operation* source, Operation* destination);

    const Operation* Source() const;
    const Operation* Destination() const;
};
