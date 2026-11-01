#ifndef VNLC_PRIMITIVE_TYPE_ANNOTATION_KIND_HPP
#define VNLC_PRIMITIVE_TYPE_ANNOTATION_KIND_HPP

#include <cstdint>

namespace vnlc {
    enum class PrimitiveTypeAnnotationKind : std::uint8_t {
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

#endif // VNLC_PRIMITIVE_TYPE_ANNOTATION_KIND_HPP
