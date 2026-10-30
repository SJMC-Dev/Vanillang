#ifndef VNLC_PRIMITIVE_TYPE_EXPRESSION_KIND_HPP
#define VNLC_PRIMITIVE_TYPE_EXPRESSION_KIND_HPP

#include <cstdint>

namespace vnlc {
    enum class PrimitiveTypeExpressionKind : std::uint8_t {
        BYTE,
        SHORT,
        INT,
        LONG,
        FLOAT,
        DOUBLE,
        BOOLEAN,
        STRING,
    };
} // namespace vnlc

#endif // VNLC_PRIMITIVE_TYPE_EXPRESSION_KIND_HPP
