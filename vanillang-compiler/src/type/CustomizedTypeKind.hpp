#ifndef VNLC_CUSTOMIZED_TYPE_KIND_HPP
#define VNLC_CUSTOMIZED_TYPE_KIND_HPP

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

#endif // VNLC_CUSTOMIZED_TYPE_KIND_HPP
