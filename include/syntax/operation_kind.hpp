#pragma once

enum class OperationKind
{
    Unknown,
    Command,
    PipeRedirection,
    FileRedirection,
    Invocation,
    Argument,
    RawStringLiteral,
    EnvironmentVariableReference,
    Concatenation,
    Word,
};
