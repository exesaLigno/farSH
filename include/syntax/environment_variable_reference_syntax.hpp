#pragma once

#include "syntax_node.hpp"

class EnvironmentVariableReferenceSyntax : public SyntaxNode
{
protected:
    void DumpNodeTo(FILE* fd) override;

public:
    EnvironmentVariableReferenceSyntax();
    ~EnvironmentVariableReferenceSyntax();
};
