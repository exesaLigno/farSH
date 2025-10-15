#pragma once

#include "operation_kind.hpp"
#include "operation.hpp"

class CompositionOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) override;
    void DumpEdgesTo(FILE* fd) override;

public:
    CompositionOperation();
};
