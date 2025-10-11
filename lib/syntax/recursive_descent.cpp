#include "syntax/syntax_tree.hpp"
#include "syntax.hpp"

void SkipSpasingSymbols(const UnicodeString& string, size_t& parser_idx);

SyntaxNode* GetPipeRedirectionSyntax(const UnicodeString& string, size_t& parser_idx);
SyntaxNode* GetCommandSyntax(const UnicodeString& string, size_t& parser_idx);
SyntaxNode* GetEnvironmentVariableReferenceSyntax(const UnicodeString& string, size_t& parser_idx);
SyntaxNode* GetRawStringLiteralSyntax(const UnicodeString& string, size_t& parser_idx);
SyntaxNode* GetWordSyntax(const UnicodeString& string, size_t& parser_idx);



SyntaxNode* SyntaxTree::Parse(const UnicodeString& string) const
{
    size_t parser_idx = 0;
    return GetPipeRedirectionSyntax(string, parser_idx);
}



void SkipSpasingSymbols(const UnicodeString& string, size_t& parser_idx)
{
    while (parser_idx < string.Length() and string[parser_idx] == ' ')
        parser_idx++;
}

SyntaxNode* GetPipeRedirectionSyntax(const UnicodeString& string, size_t& parser_idx)
{
    auto source_command = GetCommandSyntax(string, parser_idx);
    SkipSpasingSymbols(string, parser_idx);

    if (string[parser_idx] == '|')
    {
        auto pipe_redirect = new PipeRedirectionSyntax();
        parser_idx++;
        SkipSpasingSymbols(string, parser_idx);
        auto destination_command = GetPipeRedirectionSyntax(string, parser_idx);
        pipe_redirect->AppendChild(source_command);
        pipe_redirect->AppendChild(destination_command);

        return pipe_redirect;
    }

    return source_command;
}

SyntaxNode* GetCommandSyntax(const UnicodeString& string, size_t& parser_idx)
{
    auto command = new CommandSyntax();
    while (parser_idx < string.Length() and string[parser_idx] != '|')
    {
        command->AppendChild(GetEnvironmentVariableReferenceSyntax(string, parser_idx));
        SkipSpasingSymbols(string, parser_idx);
    }

    return command;
}

SyntaxNode* GetEnvironmentVariableReferenceSyntax(const UnicodeString& string, size_t& parser_idx)
{
    if (string[parser_idx] == '$')
    {
        parser_idx++;
        auto variableReference = new EnvironmentVariableReferenceSyntax();
        variableReference->AppendChild(GetWordSyntax(string, parser_idx));
        return variableReference;
    }

    return GetRawStringLiteralSyntax(string, parser_idx);
}

SyntaxNode* GetRawStringLiteralSyntax(const UnicodeString& string, size_t& parser_idx)
{
    if (string[parser_idx] == '\'')
    {
        size_t value_start = ++parser_idx;
        while (parser_idx < string.Length() and string[parser_idx] != '\'')
            parser_idx++;
        return new RawStringLiteralSyntax(string.ToString(value_start, parser_idx++));
    }

    return GetWordSyntax(string, parser_idx);
}

SyntaxNode* GetWordSyntax(const UnicodeString& string, size_t& parser_idx)
{
    size_t word_start = parser_idx;
    while (parser_idx < string.Length() and string[parser_idx] != ' ')
        parser_idx++;
    return new WordSyntax(string.ToString(word_start, parser_idx));
}
