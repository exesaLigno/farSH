#include "syntax/environment_variable_reference_syntax.hpp"

#include <cstring>

EnvironmentVariableReferenceSyntax::EnvironmentVariableReferenceSyntax() : SyntaxNode(SyntaxKind::EnvironmentVariableReference) { }

EnvironmentVariableReferenceSyntax::~EnvironmentVariableReferenceSyntax()
{
    SyntaxNode::~SyntaxNode();
}

void EnvironmentVariableReferenceSyntax::DumpNodeTo(FILE* fd)
{
    fprintf(fd, "\tnode_%x [shape=record; label=\"EnvironmentVariableReferenceSyntax\"]\n", this);
}
