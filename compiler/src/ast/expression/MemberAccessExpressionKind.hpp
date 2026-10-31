#ifndef VNLC_MEMBER_ACCESS_EXPRESSION_KIND_HPP
#define VNLC_MEMBER_ACCESS_EXPRESSION_KIND_HPP

#include <cstdint>

namespace vnlc {
    enum class MemberAccessExpressionKind : std::uint8_t {
        DOT,
        OPTIONAL_CHAINING,
    };
}

#endif // VNLC_MEMBER_ACCESS_EXPRESSION_KIND_HPP