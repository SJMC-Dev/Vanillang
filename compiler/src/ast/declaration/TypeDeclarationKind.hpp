#ifndef VNLC_TYPE_DECLARATION_KIND_HPP
#define VNLC_TYPE_DECLARATION_KIND_HPP

#include <cstdint>

namespace vnlc {
    namespace TypeDeclarationKind {
        enum class AccessModifier : std::uint8_t {
            PUBLIC,
            PROTECTED,
            PRIVATE,
        };
    }; // namespace TypeDeclarationKind
} // namespace vnlc

#endif // VNLC_TYPE_DECLARATION_KIND_HPP
