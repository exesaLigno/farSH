#include "syntax_parser.hpp"

#include <cassert>
#include <cstring>

inline bool IsWordTerminator(char symbol, bool is_quoted_string)
{
    switch (symbol)
    {
        case '"':
            return is_quoted_string;
        case ' ':
        case '\n':
        case '\t':
        case '&':
        case '|':
        case ';':
        case '>':
        case '<':
            return not is_quoted_string;
        case '\0':
        case '$':
            return true;
        default:
            return false;
    }
}

inline bool IsConcatenationTerminator(char symbol, bool is_quoted_string)
{
    return IsWordTerminator(symbol, is_quoted_string) and symbol != '$';
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

int Parser::CalculateLengthUntil(const char* string, const int start_idx, const std::function<bool(char)> is_allowed, const bool use_escaping)
{
    bool escaped = false;
    int length = 0;

    while ((escaped or is_allowed(string[start_idx + length])) and string[start_idx + length] != '\0')
    {
        if (use_escaping and string[start_idx + length] == '\\')
            escaped = true;
        else if (use_escaping)
            escaped = false;
        length++;
    }

    return length;
}

Operation* Parser::Parse(const char* string)
{
    int idx = 0;

    return ParseAndOr(string, idx);
}

void Parser::SkipSpaces(const char* string, int& idx)
{
    while (string[idx] == ' ')
        idx++;
}

Operation* Parser::ParseAndOr(const char* string, int& idx)
{
    SkipSpaces(string, idx);
    auto lhs = ParsePipeRedirection(string, idx);
    SkipSpaces(string, idx);

    if (string[idx] == '&' and string[idx + 1] == '&')
    {
        idx += 2;
        SkipSpaces(string, idx);
        auto rhs = ParseAndOr(string, idx);

        return new AndOperation(lhs, rhs);
    }

    else if (string[idx] == '|' and string[idx + 1] == '|')
    {
        idx += 2;
        SkipSpaces(string, idx);
        auto rhs = ParseAndOr(string, idx);

        return new OrOperation(lhs, rhs);
    }

    else
        return lhs;
}

Operation* Parser::ParsePipeRedirection(const char* string, int& idx)
{
    SkipSpaces(string, idx);
    auto lhs = ParseFileRedirection(string, idx);
    SkipSpaces(string, idx);

    if (string[idx] != '|' or string[idx + 1] == '|')
        return lhs;

    auto pipe_redirection = new PipeRedirectionOperation();
    pipe_redirection->AddOperand(lhs);

    while (string[idx] == '|' and string[idx + 1] != '|')
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

    while (string[idx] != '|' and string[idx] != '&' and string[idx] != '>' and string[idx] != '\0')
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

    bool is_quoted_string = string[idx] == '"';

    if (is_quoted_string)
        idx++;

    auto first = ParseEnvironmentVariableLoad(string, idx, is_quoted_string);

    if (IsConcatenationTerminator(string[idx], is_quoted_string))
    {
        if (is_quoted_string)
            idx++;
        return first;
    }

    auto concatenation = new ConcatenationOperation();
    concatenation->AddValue(first);

    while (not IsConcatenationTerminator(string[idx], is_quoted_string))
        concatenation->AddValue(ParseEnvironmentVariableLoad(string, idx, is_quoted_string));

    if (is_quoted_string)
        idx++;

    return concatenation;
}

Operation* Parser::ParseEnvironmentVariableLoad(const char* string, int& idx, bool is_quoted_string)
{
    if (string[idx] == '$')
    {
        idx++;
        auto variable_name = ParseEnvironmentVariableNameOrSubshell(string, idx, is_quoted_string);
        return new EnvironmentVariableLoadOperation(variable_name);
    }

    return ParseWord(string, idx, is_quoted_string);
}

Operation* Parser::ParseEnvironmentVariableNameOrSubshell(const char* string, int& idx, bool is_quoted_string)
{
    if (string[idx] == '{')
        return ParseEscapedEnvironmentVariableName(string, idx, is_quoted_string);

    else if (string[idx] == '(')
        throw std::runtime_error("N/I");

    else
        return ParseEnvironmentVariableName(string, idx, is_quoted_string);
}

Operation* Parser::ParseEnvironmentVariableName(const char* string, int& idx, bool is_quoted_string)
{
    int length = CalculateLengthUntil(string, idx, IsVariableNameSymbol, false);

    char buffer[length + 1];
    strncpy(buffer, string + idx, length);
    buffer[length] = 0;

    idx += length;

    return new WordOperation(buffer);
}

Operation* Parser::ParseEscapedEnvironmentVariableName(const char* string, int& idx, bool is_quoted_string)
{
    idx++;

    int length = CalculateLengthUntil(string, idx, [](char symbol) { return symbol != '}' and IsVariableNameSymbol(symbol); }, false);

    char buffer[length + 1];
    strncpy(buffer, string + idx, length);
    buffer[length] = 0;

    idx += length + 1;

    return new WordOperation(buffer);
}

Operation* Parser::ParseWord(const char* string, int& idx, bool is_quoted_string)
{
    int length = CalculateLengthUntil(string, idx, [is_quoted_string](char symbol) { return not IsWordTerminator(symbol, is_quoted_string); }, true);

    char buffer[length + 1];
    strncpy(buffer, string + idx, length);
    buffer[length] = 0;

    idx += length;

    return new WordOperation(buffer);
}
