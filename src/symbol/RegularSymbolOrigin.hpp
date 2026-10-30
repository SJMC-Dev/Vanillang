#ifndef VNLC_REGULAR_SYMBOL_ORIGIN_HPP
#define VNLC_REGULAR_SYMBOL_ORIGIN_HPP

#include <cstdint>

namespace vnlc {
    enum class RegularSymbolOrigin : std::uint8_t {
        LOCAL,
        IMPORTED,
    };
}

#endif // VNLC_REGULAR_SYMBOL_ORIGIN_HPP
