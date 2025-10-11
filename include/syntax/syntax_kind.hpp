#pragma once

enum class SyntaxKind
{
    Unknown,
    Word,
    EnvironmentVariableReference,
    RawStringLiteral,
    PipeRedirection,
    Command,
};
