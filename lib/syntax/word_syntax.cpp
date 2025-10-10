#include "syntax/word_syntax.hpp"

#include <cstring>

WordSyntax::WordSyntax(const char* _word) : SyntaxNode(SyntaxKind::Word) 
{
    word = new char[strlen(_word) + 1] { 0 };
    strcpy(word, _word);
}

WordSyntax::~WordSyntax()
{
    if (word)
    {
        delete[] word;
        word = nullptr;
    }

    SyntaxNode::~SyntaxNode();
}

void WordSyntax::DumpNodeTo(FILE* fd)
{
    fprintf(fd, "\tnode_%x [label=\"Its a Word syntax!\"]\n", this);
}

