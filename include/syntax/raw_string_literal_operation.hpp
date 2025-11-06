#pragma once

#include "operation_kind.hpp"
#include "operation.hpp"

class RawStringLiteralOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) const override;
    void DumpEdgesTo(FILE* fd) const override;

    const char* text = nullptr;

public:
    RawStringLiteralOperation(const char* _text);
    const char* GetText() const;
};
