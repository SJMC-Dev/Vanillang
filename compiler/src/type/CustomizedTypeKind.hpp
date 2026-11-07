#pragma once

#include <cstdint>

namespace vnlc {
    enum class CustomizedTypeKind : std::uint8_t {
        CLASS,
        INTERFACE,
        ENUM,
        ENUM_MEMBER,
        GENERIC_PARAMETER,
    };
}
