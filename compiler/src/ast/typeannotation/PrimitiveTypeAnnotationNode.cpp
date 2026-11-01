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
            case PrimitiveTypeAnnotationKind::SHORT:
                return PrimitiveTypeNames::Short;
            case PrimitiveTypeAnnotationKind::INT:
                return PrimitiveTypeNames::Int;
            case PrimitiveTypeAnnotationKind::LONG:
                return PrimitiveTypeNames::Long;
            case PrimitiveTypeAnnotationKind::FLOAT:
                return PrimitiveTypeNames::Float;
            case PrimitiveTypeAnnotationKind::DOUBLE:
                return PrimitiveTypeNames::Double;
            case PrimitiveTypeAnnotationKind::BOOLEAN:
                return PrimitiveTypeNames::Boolean;
            case PrimitiveTypeAnnotationKind::STRING:
                return PrimitiveTypeNames::String;
        }
        return {};
    }
} // namespace vnlc
