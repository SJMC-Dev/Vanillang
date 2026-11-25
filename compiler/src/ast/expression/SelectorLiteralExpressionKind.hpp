#pragma once

#include <cstdint>

namespace vnlc {
    enum class SelectorLiteralExpressionKind : std::uint8_t {
        NEAREST_PLAYER,   // @p
        RANDOM_PLAYER,    // @r
        ALL_PLAYERS,      // @a
        ALL_ENTITIES,     // @e
        CURRENT_EXECUTOR, // @s
        NEAREST_ENTITY,   // @n
    };
}
