#pragma once

#include "syntax_node.hpp"

class PipeRedirectionSyntax : public SyntaxNode
{
protected:
    void DumpNodeTo(FILE* fd) override;
    void DumpEdgesTo(FILE* fd) override;

public:
    PipeRedirectionSyntax();
    ~PipeRedirectionSyntax();

    SyntaxNode* Source();
    SyntaxNode* Destination();
};
