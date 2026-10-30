#ifndef VNLC_OPERATOR_DECLARATION_KIND_HPP
#define VNLC_OPERATOR_DECLARATION_KIND_HPP

#include <cstdint>

namespace vnlc {
    namespace OperatorDeclarationKind {
        enum class Kind : std::uint8_t {
            ADDITION,
            SUBTRACTION,
            MULTIPLICATION,
            DIVISION,
            INTEGER_DIVISION,
            MODULO,
            EXPONENT,
            EQUAL,
            COMPARISON,
            BITWISE_AND,
            BITWISE_OR,
            BITWISE_XOR,
            BITWISE_NOT,
            SHIFT_LEFT,
            SHIFT_RIGHT,
            SHIFT_RIGHT_UNSIGNED,
            CALL,
            SUBSCRIPT,
            RANGE,
        };

        enum class Implementation : std::uint8_t {
            REGULAR,
            NATIVE,
        };

        enum class Context : std::uint8_t {
            CLASS,
            INTERFACE,
        };

        enum class AccessModifier : std::uint8_t {
            PUBLIC,
            PROTECTED,
            PRIVATE,
        };
    }; // namespace OperatorDeclarationKind
} // namespace vnlc

#endif // VNLC_OPERATOR_DECLARATION_KIND_HPP
