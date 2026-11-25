#pragma once

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
