#ifndef VNLC_PRIMITIVE_TYPE_KIND_HPP
#define VNLC_PRIMITIVE_TYPE_KIND_HPP

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

#endif // VNLC_PRIMITIVE_TYPE_KIND_HPP