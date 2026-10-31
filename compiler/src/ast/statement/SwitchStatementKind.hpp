#ifndef VNLC_SWITCH_STATEMENT_KIND_HPP
#define VNLC_SWITCH_STATEMENT_KIND_HPP

#include <cstdint>

namespace vnlc {
    enum class SwitchStatementKind : std::uint8_t {
        LITERAL_MATCH,
        TYPE_MATCH,
    };
}

#endif // VNLC_SWITCH_STATEMENT_KIND_HPP