#ifndef VNLC_REGULAR_SYMBOL_ACCESS_MODIFIER_HPP
#define VNLC_REGULAR_SYMBOL_ACCESS_MODIFIER_HPP

#include <cstdint>

namespace vnlc {
    enum class RegularSymbolAccessModifier : std::uint8_t {
        PUBLIC,
        PROTECTED,
        PRIVATE,
    };
}

#endif // VNLC_REGULAR_SYMBOL_ACCESS_MODIFIER_HPP
