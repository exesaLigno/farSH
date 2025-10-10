#include "syntax_node.hpp"

class SyntaxTree
{
private:
    SyntaxNode* root = nullptr;

public:
    SyntaxTree();
    ~SyntaxTree();
    
    SyntaxTree(const SyntaxTree& other) = delete;
    SyntaxTree& operator=(const SyntaxTree& other) = delete;

    SyntaxTree(SyntaxTree&& other);
    SyntaxTree& operator=(SyntaxTree&& other);

    SyntaxNode* Root();
};
