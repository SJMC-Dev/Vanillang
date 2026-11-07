#pragma once

#include <cstdint>

namespace vnlc {
    enum class InheritanceOrigin : std::uint8_t {
        LOCAL,
        IMPORTED,
    };
} // namespace vnlc
