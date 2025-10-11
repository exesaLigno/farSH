#include "syntax/command_syntax.hpp"

CommandSyntax::CommandSyntax() : SyntaxNode(SyntaxKind::Command) { }
CommandSyntax::~CommandSyntax()
{
    SyntaxNode::~SyntaxNode();
}

void CommandSyntax::DumpNodeTo(FILE* fd)
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"CommandSyntax\"]\n", this);
}

void CommandSyntax::DumpEdgesTo(FILE* fd)
{
    for (size_t idx = 0; idx < children_len; idx++)
        fprintf(fd, "\tnode_%x -> node_%x [label=\"%s\"]\n", this, children[idx], children[idx] == Command() ? "Command" : "Argument");
}

const SyntaxNode* CommandSyntax::Command()
{
    return GetChild(0);
}

const SyntaxNode** CommandSyntax::Arguments()
{
    return (const SyntaxNode**) children + 1;
}
