#pragma once

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
