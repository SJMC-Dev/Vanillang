#pragma once

#include <cstdint>

namespace vnlc {
    enum class StringLiteralExpressionKind : std::uint8_t {
        STRING,
        FORMAT_STRING,
        RAW_STRING,
    };
}
