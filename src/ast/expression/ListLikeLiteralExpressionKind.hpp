#ifndef VNLC_LIST_LIKE_LITERAL_EXPRESSION_KIND_HPP
#define VNLC_LIST_LIKE_LITERAL_EXPRESSION_KIND_HPP

#include <cstdint>

namespace vnlc {
    enum class ListLikeLiteralExpressionKind : std::uint8_t {
        LIST,

        BYTE_SNBT_ARRAY,
        INT_SNBT_ARRAY,
        LONG_SNBT_ARRAY,
    };
}

#endif // VNLC_LIST_LIKE_LITERAL_EXPRESSION_KIND_HPP