#pragma once

#include <cstdint>

namespace vnlc {
    enum class AccessModifier : std::uint8_t {
        PUBLIC,
        PROTECTED,
        PRIVATE,
    };

    enum class Context : std::uint8_t {
        MODULE,
        FUNCTION,
        CLASS,
        INTERFACE,
        ENUM,
        ENUM_MEMBER,
        OPAQUE,
        BLOCK,
    };

    enum class GenericConstraintKind : std::uint8_t {
        NOTNONE,
        EXTENDS,
        IMPLEMENTS,
        HASOP,
    };

    enum class ValueKind : std::uint8_t {
        VARIABLE,
        CONSTANT,
        LOOP_VARIABLE,
        PARAMETER,
        INSTANCE_PROPERTY,
        STATIC_PROPERTY,
        ENUM_ASSOCIATED_VALUE,
    };

    enum class FunctionKind : std::uint8_t {
        FUNCTION,
        OPERATOR,
        CONSTRUCTOR,
    };

    enum class FunctionImplementation : std::uint8_t {
        REGULAR,
        NATIVE,
    };

    enum class FunctionReceiverKind : std::uint8_t {
        INSTANCE,
        STATIC,
    };

    enum class FunctionBinidingTime : std::uint8_t {
        COMPILE_TIME,
        RUNTIME,
    };

    enum class OperatorName : std::uint8_t {
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

    enum class ExpressionKeyword : std::uint8_t {
        BYTE,
        UNSIGNED_BYTE,
        SHORT,
        UNSIGNED_SHORT,
        INT,
        UNSIGNED_INT,
        LONG,
        UNSIGNED_LONG,
        FLOAT,
        DOUBLE,
        BOOLEAN,
        THIS,
        SUPER,
        NONE,
        CALLEE,
    };

    enum class UnaryExpressionKind : std::uint8_t {
        UNARY_PLUS,
        UNARY_MINUS,
        LOGICAL_NOT,
        BITWISE_NOT,
    };

    enum class BinaryExpressionKind : std::uint8_t {
        ADDITION,
        SUBTRACTION,
        MULTIPLICATION,
        DIVISION,
        INTEGER_DIVISION,
        MODULO,
        EXPONENT,
        EQUAL,
        REFERENCE_EQUAL,
        NOT_EQUAL,
        REFERENCE_NOT_EQUAL,
        GREATER_THAN,
        GREATER_THAN_OR_EQUAL,
        LESS_THAN,
        LESS_THAN_OR_EQUAL,
        COMPARISON,
        LOGICAL_AND,
        LOGICAL_OR,
        BITWISE_AND,
        BITWISE_OR,
        BITWISE_XOR,
        SHIFT_LEFT,
        SHIFT_RIGHT,
        SHIFT_RIGHT_UNSIGNED,
        NULLISH_COALESCING,
    };

    enum class PrimitiveTypeKind : std::uint8_t {
        BYTE,
        UNSIGNED_BYTE,
        SHORT,
        UNSIGNED_SHORT,
        INT,
        UNSIGNED_INT,
        LONG,
        UNSIGNED_LONG,
        FLOAT,
        DOUBLE,
        BOOLEAN,
    };

    enum class ListLikeLiteralKind : std::uint8_t {
        LIST,

        BYTE_SNBT_ARRAY,
        INT_SNBT_ARRAY,
        LONG_SNBT_ARRAY,
    };

    enum class StringLiteralKind : std::uint8_t {
        STRING,
        FORMAT_STRING,
        RAW_STRING,
    };

    enum class SelectorKind : std::uint8_t {
        NEAREST_PLAYER,   // @p
        RANDOM_PLAYER,    // @r
        ALL_PLAYERS,      // @a
        ALL_ENTITIES,     // @e
        CURRENT_EXECUTOR, // @s
        NEAREST_ENTITY,   // @n
    };

    enum class AssignmentKind : std::uint8_t {
        ASSIGNMENT,                      // =
        NULLISH_COALESCING_ASSIGNMENT,   // ??=
        ADDITION_ASSIGNMENT,             // +=
        SUBTRACTION_ASSIGNMENT,          // -=
        MULTIPLICATION_ASSIGNMENT,       // *=
        DIVISION_ASSIGNMENT,             // /=
        INTEGER_DIVISION_ASSIGNMENT,     // //=
        MODULO_ASSIGNMENT,               // %=
        EXPONENT_ASSIGNMENT,             // **=
        BITWISE_AND_ASSIGNMENT,          // &=
        BITWISE_OR_ASSIGNMENT,           // |=
        BITWISE_XOR_ASSIGNMENT,          // ^=
        SHIFT_LEFT_ASSIGNMENT,           // <<=
        SHIFT_RIGHT_ASSIGNMENT,          // >>=
        SHIFT_RIGHT_UNSIGNED_ASSIGNMENT, // >>>=
    };
} // namespace vnlc