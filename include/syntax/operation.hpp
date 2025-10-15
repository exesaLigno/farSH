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

    virtual void DumpNodeTo(FILE* fd);
    virtual void DumpEdgesTo(FILE* fd);
    void DumpSubgraphTo(FILE* fd);

public:
    ~Operation();

    OperationKind Kind();
    bool OfKind(OperationKind _kind);
    bool OfKind(std::initializer_list<OperationKind> _kinds);

    void AppendChild(Operation* child);
    size_t ChildrenCount();
    Operation* GetChild(size_t child_idx);

    Operation* GetParent();

    void DumpTo(FILE* fd);
};
