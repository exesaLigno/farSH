#pragma once

#include <cstdint>

#include "operation_kind.hpp"
#include "operation.hpp"

class FileRedirectionOperation : public Operation
{
public:
    enum Flags
    {
        NONE = 0,
        F_APPEND = 1 << 1,
        F_REDIRECT_STDOUT = 1 << 2,
        F_REDIRECT_STDERR = 1 << 3,
    };

private:
    uint8_t redirectionFlags = 0;

protected:
    void DumpNodeTo(FILE* fd) const override;
    void DumpEdgesTo(FILE* fd) const override;

public:
    FileRedirectionOperation(bool append = false);

    const Operation* Source() const;
    const Operation* Destination() const;

    bool HasFlag(uint8_t flag) const;
};
