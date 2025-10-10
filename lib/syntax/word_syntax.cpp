#include "syntax/word_syntax.hpp"

WordSyntax::WordSyntax() : SyntaxNode(SyntaxKind::Word) { }

void WordSyntax::DumpNodeTo(FILE* fd)
{
    fprintf(fd, "\tnode_%x [label=\"Its a Word syntax!\"]\n", this);
}

