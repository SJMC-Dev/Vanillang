#ifndef VNLC_SCOPE_KIND_HPP
#define VNLC_SCOPE_KIND_HPP

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
        LOOP,
        SWITCH,
    };
}

#endif // VNLC_SCOPE_KIND_HPP