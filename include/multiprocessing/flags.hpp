#pragma once

#include <cstdint>
#include <type_traits>

namespace Multiprocessing
{
    enum class Flags : uint8_t
    {
        Empty = 0,
        CaptureSTDIN = 1 << 0,
        CaptureSTDOUT = 1 << 1,
        CaptureSTDERR = 1 << 2,

        Mask = CaptureSTDIN | CaptureSTDOUT | CaptureSTDERR,
    };

    inline constexpr Flags operator&(Flags f1, Flags f2)
    {
        return static_cast<Flags>(
            static_cast<std::underlying_type_t<Flags>>(f1) &
            static_cast<std::underlying_type_t<Flags>>(f2)
        );
    }

    inline constexpr Flags operator|(Flags f1, Flags f2)
    {
        return static_cast<Flags>(
            static_cast<std::underlying_type_t<Flags>>(f1) |
            static_cast<std::underlying_type_t<Flags>>(f2)
        );
    }
}
