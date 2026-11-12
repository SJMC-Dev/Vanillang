#pragma once

#include <cstdint>

namespace vnlc {
    enum class ListLikeLiteralExpressionKind : std::uint8_t {
        LIST,

        BYTE_SNBT_ARRAY,
        INT_SNBT_ARRAY,
        LONG_SNBT_ARRAY,
    };
}
