#pragma once

#include <cstdint>

namespace vnlc {
    enum class ScopeKind : std::uint8_t {
        PACKAGE,
        MODULE,
        CLASS,
        INTERFACE,
        ENUM,
        ENUM_MEMBER,
        TYPE_ALIAS,
        FUNCTION,
        BLOCK,
        CONDITIONAL,
        LOOP,
        SWITCH,
    };
}
