#include "syntax_parser.hpp"

#include <cassert>
#include <cstring>

inline bool IsWordTerminator(char symbol)
{
    switch (symbol)
    {
        case '&':
        case '$':
        case '|':
        case ';':
        case '>':
        case '<':
        case ' ':
        case '\n':
        case '\t':
        case '\0':
            return true;
        default:
            return false;
    }
}

inline bool IsVariableNameSymbol(char symbol)
{
    return (
        (symbol >= 'a' and symbol <= 'z') or
        (symbol >= 'A' and symbol <= 'Z') or
        (symbol >= '0' and symbol <= '9') or
        symbol == '_'
    );
}

Operation* Parser::Parse(const char* string)
{
    int idx = 0;

    return ParsePipeRedirection(string, idx);
}

void Parser::SkipSpaces(const char* string, int& idx)
{
    while (string[idx] == ' ')
        idx++;
}

Operation* Parser::ParsePipeRedirection(const char* string, int& idx)
{
    SkipSpaces(string, idx);
    auto lhs = ParseFileRedirection(string, idx);
    SkipSpaces(string, idx);

    if (string[idx] != '|')
        return lhs;

    auto pipe_redirection = new PipeRedirectionOperation();
    pipe_redirection->AddOperand(lhs);

    while (string[idx] == '|')
    {
        idx++;
        SkipSpaces(string, idx);
        auto next = ParseFileRedirection(string, idx);
        SkipSpaces(string, idx);

        pipe_redirection->AddOperand(next);
    }

    return pipe_redirection;
}

Operation* Parser::ParseFileRedirection(const char* string, int& idx)
{
    SkipSpaces(string, idx);
    auto lhs = ParseInvocation(string, idx);
    SkipSpaces(string, idx);

    if (string[idx] == '>')
    {
        idx++;
        SkipSpaces(string, idx);
        auto rhs = ParseConcatenation(string, idx);
        SkipSpaces(string, idx);

        return new FileRedirectionOperation(lhs, rhs);
    }

    return lhs;
}

Operation* Parser::ParseInvocation(const char* string, int& idx)
{
    SkipSpaces(string, idx);
    auto name = ParseConcatenation(string, idx);
    InvocationOperation* invocation = new InvocationOperation(name);
    SkipSpaces(string, idx);

    while (string[idx] != '|' and string[idx] != '>' and string[idx] != '\0')
    {
        auto argument = ParseConcatenation(string, idx);
        invocation->AddArgument(argument);
        SkipSpaces(string, idx);
    }

    return invocation;
}

Operation* Parser::ParseConcatenation(const char* string, int& idx)
{
    SkipSpaces(string, idx);

    auto first = ParseEnvironmentVariableLoad(string, idx);

    if (IsWordTerminator(string[idx]) and string[idx] != '$')
        return first;

    auto concatenation = new ConcatenationOperation();
    concatenation->AddValue(first);

    while (not IsWordTerminator(string[idx]) or string[idx] == '$')
        concatenation->AddValue(ParseEnvironmentVariableLoad(string, idx));

    return concatenation;
}

Operation* Parser::ParseEnvironmentVariableLoad(const char* string, int& idx)
{
    SkipSpaces(string, idx);

    if (string[idx] == '$')
    {
        idx++;
        auto variable_name = ParseEnvironmentVariableNameOrSubshell(string, idx); // TODO: replace with method to parse variable names
        return new EnvironmentVariableLoadOperation(variable_name);
    }

    return ParseWord(string, idx);
}

Operation* Parser::ParseEnvironmentVariableNameOrSubshell(const char* string, int& idx)
{
    if (string[idx] == '{')
        return ParseEscapedEnvironmentVariableName(string, idx);

    else if (string[idx] == '(')
        throw std::runtime_error("N/I");

    else
        return ParseEnvironmentVariableName(string, idx);
}

Operation* Parser::ParseEnvironmentVariableName(const char* string, int& idx)
{
    int word_start_idx = idx;

    while (IsVariableNameSymbol(string[idx]))
        idx++;

    char buffer[idx - word_start_idx + 1];
    strncpy(buffer, string + word_start_idx, idx - word_start_idx);
    buffer[idx - word_start_idx] = 0;

    return new WordOperation(buffer);
}

Operation* Parser::ParseEscapedEnvironmentVariableName(const char* string, int& idx)
{
    idx++;

    int word_start_idx = idx;

    while (string[idx] != '}')
        idx++;

    char buffer[idx - word_start_idx + 1];
    strncpy(buffer, string + word_start_idx, idx - word_start_idx);
    buffer[idx - word_start_idx] = 0;

    idx++;

    return new WordOperation(buffer);
}

Operation* Parser::ParseWord(const char* string, int& idx)
{
    SkipSpaces(string, idx);

    int word_start_idx = idx;

    while (not IsWordTerminator(string[idx]))
        idx++;

    char buffer[idx - word_start_idx + 1];
    strncpy(buffer, string + word_start_idx, idx - word_start_idx);
    buffer[idx - word_start_idx] = 0;

    return new WordOperation(buffer);
}
