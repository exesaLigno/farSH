#pragma once

#include "operation_kind.hpp"
#include "operation.hpp"

class WordOperation : public Operation
{
protected:
    void DumpNodeTo(FILE* fd) override;
    void DumpEdgesTo(FILE* fd) override;

    const char* text = nullptr;

public:
    WordOperation(const char* _text);
};
