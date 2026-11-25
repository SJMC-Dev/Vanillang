#pragma once

#include <cstdint>

namespace vnlc {
    enum class CustomizedTypeOrigin : std::uint8_t {
        LOCAL,
        IMPORTED,
    };
}
