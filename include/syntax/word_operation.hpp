#pragma once

#include "operation_kind.hpp"
#include "operation.hpp"

class WordOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) const override;
    void DumpEdgesTo(FILE* fd) const override;

    const char* text = nullptr;

public:
    WordOperation(const char* _text);
    const char* GetText() const;
};
