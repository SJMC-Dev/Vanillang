#pragma once

#include <cstdint>

namespace vnlc {
    enum class RegularSymbolAccessModifier : std::uint8_t {
        PUBLIC,
        PROTECTED,
        PRIVATE,
    };
}
