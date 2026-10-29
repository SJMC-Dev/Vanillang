#include "PrimitiveTypeReferenceNode.hpp"
#include "type/PrimitiveTypeNames.hpp"

namespace vnlc {
    PrimitiveTypeReferenceNode::PrimitiveTypeReferenceNode(PrimitiveTypeReferenceKind kind, bool questionMarkSuffix, const Token& firstToken, const Token& lastToken) noexcept
        : TypeReferenceNode(questionMarkSuffix, firstToken, lastToken),
          kind(kind) {}

    const PrimitiveTypeReferenceKind PrimitiveTypeReferenceNode::getKind() const noexcept {
        return kind;
    }

    std::string_view PrimitiveTypeReferenceNode::getPrimitiveTypeName() const noexcept {
        switch (kind) {
            case PrimitiveTypeReferenceKind::BYTE:
                return PrimitiveTypeNames::Byte;
            case PrimitiveTypeReferenceKind::SHORT:
                return PrimitiveTypeNames::Short;
            case PrimitiveTypeReferenceKind::INT:
                return PrimitiveTypeNames::Int;
            case PrimitiveTypeReferenceKind::LONG:
                return PrimitiveTypeNames::Long;
            case PrimitiveTypeReferenceKind::FLOAT:
                return PrimitiveTypeNames::Float;
            case PrimitiveTypeReferenceKind::DOUBLE:
                return PrimitiveTypeNames::Double;
            case PrimitiveTypeReferenceKind::BOOLEAN:
                return PrimitiveTypeNames::Boolean;
            case PrimitiveTypeReferenceKind::STRING:
                return PrimitiveTypeNames::String;
        }
        return {};
    }
} // namespace vnlc
