#ifndef VNLC_SIMPLE_LITERAL_EXPRESSION_KIND_HPP
#define VNLC_SIMPLE_LITERAL_EXPRESSION_KIND_HPP

#include <cstdint>

namespace vnlc {
    enum class SimpleLiteralExpressionKind : std::uint8_t {
        BYTE,
        UNSIGNED_BYTE,
        SHORT,
        UNSIGNED_SHORT,
        INT,
        UNSIGNED_INT,
        LONG,
        UNSIGNED_LONG,
        FLOAT,
        DOUBLE,

        CHARACTER,

        BOOLEAN,
    };
}

#endif // VNLC_SIMPLE_LITERAL_EXPRESSION_KIND_HPP