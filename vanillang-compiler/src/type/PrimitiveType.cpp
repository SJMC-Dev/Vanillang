#include "PrimitiveType.hpp"
#include "PrimitiveTypeKind.hpp"
#include "PrimitiveTypeNames.hpp"
#include <string_view>

namespace vnlc {
    PrimitiveType::PrimitiveType(PrimitiveTypeKind primitiveKind) : primitiveKind(primitiveKind) {}

    PrimitiveTypeKind PrimitiveType::getPrimitiveKind() const noexcept {
        return primitiveKind;
    }

    std::string_view PrimitiveType::getFullTypeName() const noexcept {
        switch (primitiveKind) {
            case PrimitiveTypeKind::BYTE:
                return PrimitiveTypeNames::Byte;
            case PrimitiveTypeKind::SHORT:
                return PrimitiveTypeNames::Short;
            case PrimitiveTypeKind::INT:
                return PrimitiveTypeNames::Int;
            case PrimitiveTypeKind::LONG:
                return PrimitiveTypeNames::Long;
            case PrimitiveTypeKind::FLOAT:
                return PrimitiveTypeNames::Float;
            case PrimitiveTypeKind::DOUBLE:
                return PrimitiveTypeNames::Double;
            case PrimitiveTypeKind::BOOLEAN:
                return PrimitiveTypeNames::Boolean;
            case PrimitiveTypeKind::STRING:
                return PrimitiveTypeNames::String;
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

    const PrimitiveType* PrimitiveType::byteType() {
        static const PrimitiveType resolvedByteType{ PrimitiveTypeKind::BYTE };
        return &resolvedByteType;
    }

    const PrimitiveType* PrimitiveType::shortType() {
        static const PrimitiveType resolvedShortType{ PrimitiveTypeKind::SHORT };
        return &resolvedShortType;
    }

    const PrimitiveType* PrimitiveType::intType() {
        static const PrimitiveType resolvedIntType{ PrimitiveTypeKind::INT };
        return &resolvedIntType;
    }

    const PrimitiveType* PrimitiveType::longType() {
        static const PrimitiveType resolvedLongType{ PrimitiveTypeKind::LONG };
        return &resolvedLongType;
    }

    const PrimitiveType* PrimitiveType::floatType() {
        static const PrimitiveType resolvedFloatType{ PrimitiveTypeKind::FLOAT };
        return &resolvedFloatType;
    }

    const PrimitiveType* PrimitiveType::doubleType() {
        static const PrimitiveType resolvedDoubleType{ PrimitiveTypeKind::DOUBLE };
        return &resolvedDoubleType;
    }

    const PrimitiveType* PrimitiveType::booleanType() {
        static const PrimitiveType resolvedBooleanType{ PrimitiveTypeKind::BOOLEAN };
        return &resolvedBooleanType;
    }

    const PrimitiveType* PrimitiveType::stringType() {
        static const PrimitiveType resolvedStringType{ PrimitiveTypeKind::STRING };
        return &resolvedStringType;
    }
} // namespace vnlc
