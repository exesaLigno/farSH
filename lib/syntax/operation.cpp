#include "syntax/operation.hpp"

#include <cstring>

Operation::Operation(OperationKind _kind) : kind(_kind) { }
Operation::~Operation()
{
    if (children)
    {
        for (size_t idx = 0; idx < children_len; idx++)
            delete children[idx];
        delete[] children;
        children = nullptr;
    }
}

OperationKind Operation::Kind() const
{
    return kind;
}

bool Operation::OfKind(OperationKind _kind) const
{
    return kind == _kind;
}

bool Operation::OfKind(std::initializer_list<OperationKind> _kinds) const
{
    for (OperationKind _kind : _kinds)
        if (kind == _kind)
            return true;
    return false;
}

void Operation::AppendChild(Operation* child)
{
    size_t old_children_len = children_len;
    Operation** old_children = children;

    children_len += 1;
    children = new Operation*[children_len];

    if (old_children)
    {
        memcpy(children, old_children, sizeof(Operation*) * old_children_len);
        delete[] old_children;
    }

    children[children_len - 1] = child;
    child->parent = this;
}

size_t Operation::ChildrenCount() const
{
    return children_len;
}

Operation* Operation::GetChild(size_t idx) const
{
    return children[idx];
}

Operation* Operation::GetParent() const
{
    return parent;
}

void Operation::DumpNodeTo(FILE* fd) const
{
    fprintf(fd, "\tnode_%x\n", this, kind);
}

void Operation::DumpEdgesTo(FILE* fd) const
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x\n", this, children[idx]);
}

void Operation::DumpSubgraphTo(FILE* fd) const
{
    DumpNodeTo(fd);

    for (size_t idx = 0; idx < children_len; idx++)
        children[idx] -> DumpSubgraphTo(fd);

    DumpEdgesTo(fd);
}

void Operation::DumpTo(FILE* fd) const
{
    fputs("digraph G {\n", fd);
    DumpSubgraphTo(fd);
    fputs("}\n", fd);
}
