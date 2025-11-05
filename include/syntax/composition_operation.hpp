#pragma once

#include "operation_kind.hpp"
#include "operation.hpp"

class CompositionOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) const override;
    void DumpEdgesTo(FILE* fd) const override;

public:
    CompositionOperation();

    size_t ValuesCount() const;
    const Operation* Value(size_t idx) const;
};
