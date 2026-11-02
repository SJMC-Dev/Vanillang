#include "PrimitiveTypeAnnotationNode.hpp"
#include "type/PrimitiveTypeNames.hpp"

namespace vnlc {
    PrimitiveTypeAnnotationNode::PrimitiveTypeAnnotationNode(PrimitiveTypeAnnotationKind kind, bool questionMarkSuffix, const Token& firstToken, const Token& lastToken) noexcept
        : TypeAnnotationNode(questionMarkSuffix, firstToken, lastToken),
          kind(kind) {}

    PrimitiveTypeAnnotationKind PrimitiveTypeAnnotationNode::getKind() const noexcept {
        return kind;
    }

    std::string_view PrimitiveTypeAnnotationNode::getPrimitiveTypeName() const noexcept {
        switch (kind) {
            case PrimitiveTypeAnnotationKind::BYTE:
                return PrimitiveTypeNames::Byte;
            case PrimitiveTypeAnnotationKind::UNSIGNED_BYTE:
                return PrimitiveTypeNames::UnsignedByte;
            case PrimitiveTypeAnnotationKind::SHORT:
                return PrimitiveTypeNames::Short;
            case PrimitiveTypeAnnotationKind::UNSIGNED_SHORT:
                return PrimitiveTypeNames::UnsignedShort;
            case PrimitiveTypeAnnotationKind::INT:
                return PrimitiveTypeNames::Int;
            case PrimitiveTypeAnnotationKind::UNSIGNED_INT:
                return PrimitiveTypeNames::UnsignedInt;
            case PrimitiveTypeAnnotationKind::LONG:
                return PrimitiveTypeNames::Long;
            case PrimitiveTypeAnnotationKind::UNSIGNED_LONG:
                return PrimitiveTypeNames::UnsignedLong;
            case PrimitiveTypeAnnotationKind::FLOAT:
                return PrimitiveTypeNames::Float;
            case PrimitiveTypeAnnotationKind::DOUBLE:
                return PrimitiveTypeNames::Double;
            case PrimitiveTypeAnnotationKind::BOOLEAN:
                return PrimitiveTypeNames::Boolean;
        }
        return {};
    }
} // namespace vnlc
