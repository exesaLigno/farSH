#pragma once

#include <initializer_list>
#include <cstdlib>
#include <cstdio>

#include "operation_kind.hpp"

class Operation
{
protected:
    OperationKind kind = OperationKind::Unknown;
    Operation* parent = nullptr;
    Operation** children = nullptr;
    size_t children_len = 0;

    Operation(OperationKind _kind);

    virtual void DumpNodeTo(FILE* fd) const;
    virtual void DumpEdgesTo(FILE* fd) const;
    void DumpSubgraphTo(FILE* fd) const;

    void AppendChild(Operation* child);
    
public:
    ~Operation();

    OperationKind Kind() const;
    bool OfKind(OperationKind _kind) const;
    bool OfKind(std::initializer_list<OperationKind> _kinds) const;

    size_t ChildrenCount() const;
    Operation* GetChild(size_t child_idx) const;

    Operation* GetParent() const;

    void DumpTo(FILE* fd) const;
};
