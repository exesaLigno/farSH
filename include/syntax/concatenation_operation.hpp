#pragma once

#include "operation_kind.hpp"
#include "operation.hpp"

class ConcatenationOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) const override;
    void DumpEdgesTo(FILE* fd) const override;

public:
    ConcatenationOperation();
    ConcatenationOperation(std::initializer_list<Operation*> components);

    void AddValue(Operation* value);

    size_t ValuesCount() const;
    const Operation* Value(size_t idx) const;
};
