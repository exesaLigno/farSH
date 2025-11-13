#pragma once

#include "syntax.hpp"

class Parser
{
public:
    Operation* Parse(const char* string);

// private:
    void SkipSpaces(const char* string, int& idx);
    Operation* ParsePipeRedirection(const char* string, int& idx);
    Operation* ParseFileRedirection(const char* string, int& idx);
    Operation* ParseInvocation(const char* string, int& idx);
    Operation* ParseConcatenation(const char* string, int& idx);
    Operation* ParseEnvironmentVariableLoad(const char* string, int& idx);
    Operation* ParseWord(const char* string, int& idx);
    Operation* ParseRawString(const char* string, int& idx);
};
