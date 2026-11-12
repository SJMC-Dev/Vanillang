#pragma once

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
