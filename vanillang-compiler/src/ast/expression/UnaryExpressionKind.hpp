#ifndef VNLC_UNARY_EXPRESSION_KIND_HPP
#define VNLC_UNARY_EXPRESSION_KIND_HPP

#include <cstdint>

namespace vnlc {
    enum class UnaryExpressionKind : std::uint8_t {
        UNARY_PLUS,
        UNARY_MINUS,
        LOGICAL_NOT,
        BITWISE_NOT,
    };
}

#endif // VNLC_UNARY_EXPRESSION_KIND_HPP