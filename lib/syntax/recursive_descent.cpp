#include "syntax/syntax_tree.hpp"
#include "syntax.hpp"

void SkipSpasingSymbols(const UnicodeString& string, size_t& parser_idx);

SyntaxNode* GetCommandSyntax(const UnicodeString& string, size_t& parser_idx);
SyntaxNode* GetEnvironmentVariableReferenceSyntax(const UnicodeString& string, size_t& parser_idx);
SyntaxNode* GetRawStringLiteralSyntax(const UnicodeString& string, size_t& parser_idx);
SyntaxNode* GetWordSyntax(const UnicodeString& string, size_t& parser_idx);



SyntaxNode* SyntaxTree::Parse(const UnicodeString& string) const
{
    size_t parser_idx = 0;
    return GetCommandSyntax(string, parser_idx);
}



void SkipSpasingSymbols(const UnicodeString& string, size_t& parser_idx)
{
    while (parser_idx < string.Length() and string[parser_idx] == ' ')
        parser_idx++;
}

SyntaxNode* GetCommandSyntax(const UnicodeString& string, size_t& parser_idx)
{
    auto command = new CommandSyntax();
    while (parser_idx < string.Length())
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
