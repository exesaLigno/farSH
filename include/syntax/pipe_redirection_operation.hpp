#pragma once

#include "operation_kind.hpp"
#include "operation.hpp"

class PipeRedirectionOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) override;
    void DumpEdgesTo(FILE* fd) override;

public:
    PipeRedirectionOperation();

    const Operation* Source() const;
    const Operation* Destination() const;
};
