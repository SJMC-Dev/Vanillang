#pragma once

#include <cstdint>

namespace vnlc {
    enum class ScopeOrigin : std::uint8_t {
        LOCAL,
        IMPORTED,
    };
}
