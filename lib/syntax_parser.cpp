#include "syntax_parser.hpp"

#include <cassert>
#include <cstring>

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

    if (string[idx] == '|')
    {
        idx++;
        SkipSpaces(string, idx);
        auto rhs = ParsePipeRedirection(string, idx);
        SkipSpaces(string, idx);

        return new PipeRedirectionOperation(lhs, rhs);
    }

    return lhs;
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
    return ParseEnvironmentVariableLoad(string, idx);
}

Operation* Parser::ParseEnvironmentVariableLoad(const char* string, int& idx)
{
    return ParseWord(string, idx);
}

Operation* Parser::ParseWord(const char* string, int& idx)
{
    SkipSpaces(string, idx);

    int word_start_idx = idx;

    while (string[idx] != ' ' and string[idx] != '\0')
        idx++;

    char buffer[idx - word_start_idx + 1];
    strncpy(buffer, string + word_start_idx, idx - word_start_idx);
    buffer[idx - word_start_idx] = 0;

    SkipSpaces(string, idx);

    return new WordOperation(buffer);
}

Operation* Parser::ParseRawString(const char* string, int& idx)
{
    throw std::runtime_error("N/I");
}
