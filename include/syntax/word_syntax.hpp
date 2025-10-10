#include "syntax_node.hpp"

class WordSyntax : public SyntaxNode
{
private:
    char* word = nullptr;

protected:
    void DumpNodeTo(FILE* fd) override;

public:
    WordSyntax(const char* _word);
    ~WordSyntax();

    const char* Word();
};
