#include "syntax/syntax_node.hpp"

#include <cstring>

SyntaxNode::SyntaxNode() { }
SyntaxNode::SyntaxNode(SyntaxKind _kind) : kind(_kind) { }
SyntaxNode::~SyntaxNode()
{
    if (children)
    {
        for (size_t idx = 0; idx < children_len; idx++)
            delete children[idx];
        delete[] children;
        children = nullptr;
    }
}

SyntaxKind SyntaxNode::Kind()
{
    return kind;
}

bool SyntaxNode::OfKind(SyntaxKind _kind)
{
    return kind == _kind;
}

bool SyntaxNode::OfKind(std::initializer_list<SyntaxKind> _kinds)
{
    for (SyntaxKind _kind : _kinds)
        if (kind == _kind)
            return true;
    return false;
}

void SyntaxNode::AppendChild(SyntaxNode* child)
{
    size_t old_children_len = children_len;
    SyntaxNode** old_children = children;

    children_len += 1;
    children = new SyntaxNode*[children_len];

    if (old_children)
    {
        memcpy(children, old_children, sizeof(SyntaxNode*) * old_children_len);
        delete[] old_children;
    }

    children[children_len - 1] = child;
    child->parent = this;
}

size_t SyntaxNode::ChildrenCount()
{
    return children_len;
}

SyntaxNode* SyntaxNode::GetChild(size_t idx)
{
    return children[idx];
}

SyntaxNode* SyntaxNode::GetParent()
{
    return parent;
}

void SyntaxNode::DumpNodeTo(FILE* fd)
{
    fprintf(fd, "\tnode_%x\n", this, kind);
}

void SyntaxNode::DumpEdgesTo(FILE* fd)
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x\n", this, children[idx]);
}

void SyntaxNode::DumpSubgraphTo(FILE* fd)
{
    DumpNodeTo(fd);

    for (size_t idx = 0; idx < children_len; idx++)
        children[idx] -> DumpSubgraphTo(fd);

    DumpEdgesTo(fd);
}

void SyntaxNode::DumpSyntaxTo(FILE* fd)
{
    fputs("digraph G {\n", fd);
    DumpSubgraphTo(fd);
    fputs("}\n", fd);
}
