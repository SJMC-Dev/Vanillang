#pragma once

#include <cstdint>

namespace vnlc {
    enum class UnaryExpressionKind : std::uint8_t {
        UNARY_PLUS,
        UNARY_MINUS,
        LOGICAL_NOT,
        BITWISE_NOT,
    };
}
