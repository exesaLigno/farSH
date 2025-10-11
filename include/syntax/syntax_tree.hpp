#pragma once

#include "syntax_node.hpp"
#include "unicode/string.hpp"

class SyntaxTree
{
private:
    SyntaxNode* root = nullptr;

public:
    SyntaxTree(const UnicodeString& string);
    ~SyntaxTree();
    
    SyntaxTree(const SyntaxTree& other) = delete;
    SyntaxTree& operator=(const SyntaxTree& other) = delete;

    SyntaxTree(SyntaxTree&& other);
    SyntaxTree& operator=(SyntaxTree&& other);

    SyntaxNode* Root() const;

    void DumpTo(FILE* fd) const;

    SyntaxNode* Parse(const UnicodeString& string) const;
};
