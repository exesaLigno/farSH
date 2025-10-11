#pragma once

#include "syntax_node.hpp"
#include "word_syntax.hpp"

class CommandSyntax : public SyntaxNode
{
protected:
    void DumpNodeTo(FILE* fd) override;
    void DumpEdgesTo(FILE* fd) override;

public:
    CommandSyntax();
    ~CommandSyntax();

    const SyntaxNode* Command();
    const SyntaxNode** Arguments();
};
