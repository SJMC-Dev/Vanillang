#ifndef VNLC_FUNCTION_DECLARATION_KIND_HPP
#define VNLC_FUNCTION_DECLARATION_KIND_HPP

#include <cstdint>

namespace vnlc {
    namespace FunctionDeclarationKind {
        enum class Implementation : std::uint8_t {
            REGULAR,
            NATIVE,
        };

        enum class Context : std::uint8_t {
            TOP_LEVEL,
            CLASS,
            INTERFACE,
        };

        enum class AccessModifier : std::uint8_t {
            PUBLIC,
            PROTECTED,
            PRIVATE,
        };

        enum class Binding : std::uint8_t {
            INSTANCE,
            STATIC,
        };
    }; // namespace FunctionDeclarationKind
} // namespace vnlc

#endif // VNLC_FUNCTION_DECLARATION_KIND_HPP