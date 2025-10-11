#pragma once

#include "syntax_node.hpp"

class RawStringLiteralSyntax : public SyntaxNode
{
private:
    char* value = nullptr;

protected:
    void DumpNodeTo(FILE* fd) override;

public:
    RawStringLiteralSyntax(const char* _value);
    RawStringLiteralSyntax(char* _value);
    ~RawStringLiteralSyntax();

    const char* Value();
};
