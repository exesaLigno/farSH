#include "syntax/pipe_redirection_syntax.hpp"

PipeRedirectionSyntax::PipeRedirectionSyntax() : SyntaxNode(SyntaxKind::Command) { }
PipeRedirectionSyntax::~PipeRedirectionSyntax()
{
    SyntaxNode::~SyntaxNode();
}

void PipeRedirectionSyntax::DumpNodeTo(FILE* fd)
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"PipeRedirectionSyntax\"]\n", this);
}

void PipeRedirectionSyntax::DumpEdgesTo(FILE* fd)
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x [label=\"%s\"]\n", this, children[idx], children[idx] == Source() ? "Source" : "Destination");
}

SyntaxNode* PipeRedirectionSyntax::Source()
{
    return GetChild(0);
}

SyntaxNode* PipeRedirectionSyntax::Destination()
{
    return GetChild(1);
}
