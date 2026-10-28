#include "PrimitiveType.hpp"
#include "PrimitiveTypeKind.hpp"
#include <string_view>

namespace vnlc {
    PrimitiveType::PrimitiveType(PrimitiveTypeKind primitiveKind) : primitiveKind(primitiveKind) {}

    PrimitiveTypeKind PrimitiveType::getPrimitiveKind() const noexcept {
        return primitiveKind;
    }

    std::string_view PrimitiveType::getFullTypeName() const noexcept {
        switch (primitiveKind) {
            case PrimitiveTypeKind::BYTE:
                return "byte";
            case PrimitiveTypeKind::SHORT:
                return "short";
            case PrimitiveTypeKind::INT:
                return "int";
            case PrimitiveTypeKind::LONG:
                return "long";
            case PrimitiveTypeKind::FLOAT:
                return "float";
            case PrimitiveTypeKind::DOUBLE:
                return "double";
            case PrimitiveTypeKind::BOOLEAN:
                return "bool";
            case PrimitiveTypeKind::STRING:
                return "string";
        }

        return "unknown"; // should never reach here
    }

    bool PrimitiveType::canConvertTo(const Type& other) const noexcept {
        const auto* primitiveType = dynamic_cast<const PrimitiveType*>(&other);
        if (primitiveType == nullptr) return false;

        const PrimitiveTypeKind targetKind = primitiveType->primitiveKind;
        if (primitiveKind == targetKind) return true;
        if (primitiveKind == PrimitiveTypeKind::STRING || targetKind == PrimitiveTypeKind::STRING) return false;
        if (primitiveKind == PrimitiveTypeKind::BOOLEAN || targetKind == PrimitiveTypeKind::BOOLEAN) return true;

        switch (primitiveKind) {
            case PrimitiveTypeKind::BYTE:
                return targetKind == PrimitiveTypeKind::SHORT || targetKind == PrimitiveTypeKind::INT || targetKind == PrimitiveTypeKind::LONG;
            case PrimitiveTypeKind::SHORT:
                return targetKind == PrimitiveTypeKind::INT || targetKind == PrimitiveTypeKind::LONG;
            case PrimitiveTypeKind::INT:
                return targetKind == PrimitiveTypeKind::LONG;
            case PrimitiveTypeKind::FLOAT:
                return targetKind == PrimitiveTypeKind::DOUBLE;
            default:
                return false;
        }
    }

    bool PrimitiveType::canUseOperator(std::string_view name) const noexcept {
        if (name == "assignment") return true;

        constexpr std::string_view assignmentSuffix = "Assignment";
        if (name.ends_with(assignmentSuffix)) {
            const std::string_view operatorName = name.substr(0, name.size() - assignmentSuffix.size());
            const bool isCompoundAssignable = operatorName == "addition" || operatorName == "subtraction" || operatorName == "multiplication" || operatorName == "division" ||
                                              operatorName == "integerDivision" || operatorName == "modulo" || operatorName == "exponent" || operatorName == "bitwiseAnd" ||
                                              operatorName == "bitwiseOr" || operatorName == "bitwiseXor" || operatorName == "shiftLeft" || operatorName == "shiftRight" ||
                                              operatorName == "unsignedShiftRight" || operatorName == "nullishCoalescing";
            return isCompoundAssignable && canUseOperator(operatorName);
        }

        const bool isEqualityOperator = name == "equality" || name == "inequality";
        const bool isArithmeticOperator =
            name == "addition" || name == "subtraction" || name == "multiplication" || name == "division" || name == "integerDivision" || name == "modulo" || name == "exponent";
        const bool isSignOperator = name == "unaryPlus" || name == "unaryMinus";
        const bool isRelationalOperator = name == "lessThan" || name == "greaterThan" || name == "lessThanOrEqual" || name == "greaterThanOrEqual";
        const bool isLogicalBitwiseOperator = name == "bitwiseAnd" || name == "bitwiseOr" || name == "bitwiseXor";
        const bool isIntegralOnlyOperator = name == "bitwiseNot" || name == "shiftLeft" || name == "shiftRight" || name == "unsignedShiftRight" || name == "range";
        const bool isLogicalOperator = name == "logicalAnd" || name == "logicalOr" || name == "logicalNot";

        switch (primitiveKind) {
            case PrimitiveTypeKind::BYTE:
            case PrimitiveTypeKind::SHORT:
            case PrimitiveTypeKind::INT:
            case PrimitiveTypeKind::LONG:
                return isEqualityOperator || isArithmeticOperator || isSignOperator || isRelationalOperator || isLogicalBitwiseOperator || isIntegralOnlyOperator;
            case PrimitiveTypeKind::FLOAT:
            case PrimitiveTypeKind::DOUBLE:
                return isEqualityOperator || isArithmeticOperator || isSignOperator || isRelationalOperator;
            case PrimitiveTypeKind::BOOLEAN:
                return isEqualityOperator || isLogicalBitwiseOperator || isLogicalOperator;
            case PrimitiveTypeKind::STRING:
                return isEqualityOperator || name == "addition" || name == "subscript";
        }

        return false;
    }

    const PrimitiveType* PrimitiveType::BYTE_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::SHORT_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::INT_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::LONG_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::FLOAT_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::DOUBLE_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::BOOLEAN_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::STRING_TYPE = nullptr;

    const PrimitiveType* PrimitiveType::byteType() {
        if (BYTE_TYPE == nullptr) {
            BYTE_TYPE = new PrimitiveType(PrimitiveTypeKind::BYTE);
        }
        return BYTE_TYPE;
    }

    const PrimitiveType* PrimitiveType::shortType() {
        if (SHORT_TYPE == nullptr) {
            SHORT_TYPE = new PrimitiveType(PrimitiveTypeKind::SHORT);
        }
        return SHORT_TYPE;
    }

    const PrimitiveType* PrimitiveType::intType() {
        if (INT_TYPE == nullptr) {
            INT_TYPE = new PrimitiveType(PrimitiveTypeKind::INT);
        }
        return INT_TYPE;
    }

    const PrimitiveType* PrimitiveType::longType() {
        if (LONG_TYPE == nullptr) {
            LONG_TYPE = new PrimitiveType(PrimitiveTypeKind::LONG);
        }
        return LONG_TYPE;
    }

    const PrimitiveType* PrimitiveType::floatType() {
        if (FLOAT_TYPE == nullptr) {
            FLOAT_TYPE = new PrimitiveType(PrimitiveTypeKind::FLOAT);
        }
        return FLOAT_TYPE;
    }

    const PrimitiveType* PrimitiveType::doubleType() {
        if (DOUBLE_TYPE == nullptr) {
            DOUBLE_TYPE = new PrimitiveType(PrimitiveTypeKind::DOUBLE);
        }
        return DOUBLE_TYPE;
    }

    const PrimitiveType* PrimitiveType::booleanType() {
        if (BOOLEAN_TYPE == nullptr) {
            BOOLEAN_TYPE = new PrimitiveType(PrimitiveTypeKind::BOOLEAN);
        }
        return BOOLEAN_TYPE;
    }

    const PrimitiveType* PrimitiveType::stringType() {
        if (STRING_TYPE == nullptr) {
            STRING_TYPE = new PrimitiveType(PrimitiveTypeKind::STRING);
        }
        return STRING_TYPE;
    }
} // namespace vnlc
