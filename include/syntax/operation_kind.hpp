#pragma once

enum class OperationKind
{
    Unknown,
    PipeRedirection,
    FileRedirection,
    Invocation,
    RawStringLiteral,
    EnvironmentVariableReference,
    Concatenation,
    Word
};
