#ifndef VNLC_CUSTOMIZED_TYPE_ORIGIN_HPP
#define VNLC_CUSTOMIZED_TYPE_ORIGIN_HPP

#include <cstdint>

namespace vnlc {
    enum class CustomizedTypeOrigin : std::uint8_t {
        LOCAL,
        IMPORTED,
    };
}

#endif // VNLC_CUSTOMIZED_TYPE_ORIGIN_HPP