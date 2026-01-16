#pragma once

enum class OperationKind
{
    Unknown,
    PipeRedirection,
    FileRedirection,
    And, Or, Not,
    Invocation,
    EnvironmentVariableLoad,
    Concatenation,
    Word
};
