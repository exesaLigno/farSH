#pragma once

#include <initializer_list>
#include <cstdlib>
#include <cstdio>

#include "syntax_kind.hpp"

class SyntaxNode
{
protected:
    SyntaxKind kind = SyntaxKind::Unknown;
    SyntaxNode* parent = nullptr;
    SyntaxNode** children = nullptr;
    size_t children_len = 0;

    SyntaxNode(SyntaxKind _kind);

    virtual void DumpNodeTo(FILE* fd);
    virtual void DumpEdgesTo(FILE* fd);
    void DumpSubgraphTo(FILE* fd);

public:
    SyntaxNode();
    ~SyntaxNode();

    SyntaxKind Kind();
    bool OfKind(SyntaxKind _kind);
    bool OfKind(std::initializer_list<SyntaxKind> _kinds);

    void AppendChild(SyntaxNode* child);
    size_t ChildrenCount();
    SyntaxNode* GetChild(size_t child_idx);

    SyntaxNode* GetParent();

    void DumpSyntaxTo(FILE* fd);
};
