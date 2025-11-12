#pragma once

#include "operation_kind.hpp"
#include "operation.hpp"

class ConcatenationOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) const override;
    void DumpEdgesTo(FILE* fd) const override;

public:
    ConcatenationOperation(std::initializer_list<Operation*> components);

    size_t ValuesCount() const;
    const Operation* Value(size_t idx) const;
};
