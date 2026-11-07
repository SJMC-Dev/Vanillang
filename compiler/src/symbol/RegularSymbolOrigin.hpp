#pragma once

#include <cstdint>

namespace vnlc {
    enum class RegularSymbolOrigin : std::uint8_t {
        LOCAL,
        IMPORTED,
    };
}
