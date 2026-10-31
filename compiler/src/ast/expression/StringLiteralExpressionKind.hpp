#ifndef VNLC_STRING_LITERAL_EXPRESSION_KIND_HPP
#define VNLC_STRING_LITERAL_EXPRESSION_KIND_HPP

#include <cstdint>

namespace vnlc {
    enum class StringLiteralExpressionKind : std::uint8_t {
        STRING,
        FORMAT_STRING,
        RAW_STRING,
    };
}

#endif // VNLC_STRING_LITERAL_EXPRESSION_KIND_HPP