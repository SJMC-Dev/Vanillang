#ifndef VNLC_PRIMITIVE_TYPE_KIND_HPP
#define VNLC_PRIMITIVE_TYPE_KIND_HPP

#include <cstdint>

namespace vnlc {
    enum class PrimitiveTypeKind : std::uint8_t {
        BYTE,
        SHORT,
        INT,
        LONG,
        FLOAT,
        DOUBLE,
        BOOLEAN,
        STRING,
    };
}

#endif // VNLC_PRIMITIVE_TYPE_KIND_HPP