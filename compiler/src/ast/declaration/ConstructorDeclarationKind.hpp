#ifndef VNLC_CONSTRUCTOR_DECLARATION_KIND_HPP
#define VNLC_CONSTRUCTOR_DECLARATION_KIND_HPP

#include <cstdint>

namespace vnlc {
    namespace ConstructorDeclarationKind {
        enum class AccessModifier : std::uint8_t {
            PUBLIC,
            PROTECTED,
            PRIVATE,
        };
    }; // namespace ConstructorDeclarationKind
} // namespace vnlc

#endif // VNLC_CONSTRUCTOR_DECLARATION_KIND_HPP
