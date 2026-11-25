#pragma once

#include <cstdint>

namespace vnlc {
    enum class MemberAccessExpressionKind : std::uint8_t {
        DOT,
        OPTIONAL_CHAINING,
    };
}
