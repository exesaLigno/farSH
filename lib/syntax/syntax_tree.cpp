#include "syntax/syntax_tree.hpp"

SyntaxTree::SyntaxTree(const UnicodeString& string)
{
    root = Parse(string);
}

SyntaxTree::~SyntaxTree()
{
    if (root)
        delete root;
}

SyntaxTree::SyntaxTree(SyntaxTree&& other)
{
    root = other.root;
    other.root = nullptr;
}

SyntaxTree& SyntaxTree::operator=(SyntaxTree&& other)
{
    if (this != &other)
    {
        root = other.root;
        other.root = nullptr;
    }

    return *this;
}

SyntaxNode* SyntaxTree::Root() const
{
    return root;
}

void SyntaxTree::DumpTo(FILE* fd) const
{
    Root()->DumpSyntaxTo(fd);
}
