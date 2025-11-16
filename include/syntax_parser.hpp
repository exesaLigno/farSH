#pragma once
#include <functional>

#include "syntax.hpp"

class Parser
{
public:
    Operation* Parse(const char* string);

private:
    void SkipSpaces(const char* string, int& idx);

    static int CalculateLengthUntil(const char* string, const int start_idx, const std::function<bool(char)> is_allowed, const bool use_escaping = true);
    
    Operation* ParseAndOr(const char* string, int& idx);
    Operation* ParsePipeRedirection(const char* string, int& idx);
    Operation* ParseFileRedirection(const char* string, int& idx);
    Operation* ParseInvocation(const char* string, int& idx);
    Operation* ParseConcatenation(const char* string, int& idx);
    Operation* ParseEnvironmentVariableLoad(const char* string, int& idx, bool is_quoted_string);
    Operation* ParseEnvironmentVariableNameOrSubshell(const char* string, int& idx, bool is_quoted_string);
    Operation* ParseEnvironmentVariableName(const char* string, int& idx, bool is_quoted_string);
    Operation* ParseEscapedEnvironmentVariableName(const char* string, int& idx, bool is_quoted_string);
    Operation* ParseWord(const char* string, int& idx, bool is_quoted_string);
};
