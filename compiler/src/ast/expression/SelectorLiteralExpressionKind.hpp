#ifndef VNLC_SELECTOR_LITERAL_EXPRESSION_KIND_HPP
#define VNLC_SELECTOR_LITERAL_EXPRESSION_KIND_HPP

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

#endif // VNLC_SELECTOR_LITERAL_EXPRESSION_KIND_HPP