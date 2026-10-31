#ifndef VNLC_REGULAR_SYMBOL_KIND_HPP
#define VNLC_REGULAR_SYMBOL_KIND_HPP

#include <cstdint>

namespace vnlc {
    enum class RegularSymbolKind : std::uint8_t {
        PACKAGE,
        MODULE,
        VARIABLE,
        FUNCTION,
        CLASS,
        INTERFACE,
        ENUM,
        TYPE_ALIAS,
        ENUM_MEMBER,
        PROPERTY,
        METHOD,
        PARAMETER,
        GENERIC_PARAMETER,
        LOOP_LABEL,
    };
}

#endif // VNLC_REGULAR_SYMBOL_KIND_HPP
