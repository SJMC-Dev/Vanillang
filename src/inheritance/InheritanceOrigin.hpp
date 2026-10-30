#ifndef VNLC_INHERITANCE_ORIGIN_HPP
#define VNLC_INHERITANCE_ORIGIN_HPP

#include <cstdint>

namespace vnlc {
    enum class InheritanceOrigin : std::uint8_t {
        LOCAL,
        IMPORTED,
    };
} // namespace vnlc

#endif // VNLC_INHERITANCE_ORIGIN_HPP
