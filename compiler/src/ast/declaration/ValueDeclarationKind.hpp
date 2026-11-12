#pragma once

#include <cstdint>

namespace vnlc {
    namespace ValueDeclarationKind {
        enum class Kind : std::uint8_t {
            LET,
            LOOP_VARIABLE,
            PARAMETER,
            INSTANCE_PROPERTY,
            STATIC_PROPERTY,
            ENUM_ASSOCIATED_VALUE,
        };

        enum class Context : std::uint8_t {
            TOP_LEVEL,
            FUNCTION,
            CLASS,
            INTERFACE,
            ENUM_MEMBER,
            BLOCK,
        };

        enum class AccessModifier : std::uint8_t {
            PUBLIC,
            PROTECTED,
            PRIVATE,
        };
    }; // namespace ValueDeclarationKind
} // namespace vnlc
