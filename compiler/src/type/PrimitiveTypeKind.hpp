#pragma once

#include <cstdint>

namespace vnlc {
    enum class PrimitiveTypeKind : std::uint8_t {
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
        BOOLEAN,
    };
}
