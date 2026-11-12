#pragma once

#include <cstdint>

namespace vnlc {
    enum class SwitchStatementKind : std::uint8_t {
        LITERAL_MATCH,
        TYPE_MATCH,
    };
}
