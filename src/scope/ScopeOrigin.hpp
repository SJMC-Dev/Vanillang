#ifndef VNLC_SCOPE_ORIGIN_HPP
#define VNLC_SCOPE_ORIGIN_HPP

#include <cstdint>

namespace vnlc {
    enum class ScopeOrigin : std::uint8_t {
        LOCAL,
        IMPORTED,
    };
}

#endif // VNLC_SCOPE_ORIGIN_HPP
