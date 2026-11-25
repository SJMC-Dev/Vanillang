#include "TypeAnnotationNode.hpp"
#include "ast/typeannotation/CustomizedTypeAnnotationNode.hpp"
#include "ast/typeannotation/PrimitiveTypeAnnotationNode.hpp"

namespace vnlc {
    TypeAnnotationNode::TypeAnnotationNode(bool questionMarkSuffix, const Token& firstToken, const Token& lastToken) noexcept
        : AstNode(firstToken, lastToken),
          questionMarkSuffix(questionMarkSuffix) {}

    bool TypeAnnotationNode::hasQuestionMarkSuffix() const noexcept {
        return questionMarkSuffix;
    }

    std::string TypeAnnotationNode::generateInternalNamePart() const {
        if (const auto* primitiveType = dynamic_cast<const PrimitiveTypeAnnotationNode*>(this)) {
            return std::string(primitiveType->getPrimitiveTypeName());
        }

        const auto* customizedType = dynamic_cast<const CustomizedTypeAnnotationNode*>(this);
        if (customizedType == nullptr) {
            return {};
        }

        std::string identifier;
        for (const auto& part : customizedType->getNameParts()) {
            identifier += std::string(part->getIdentifierString()) + ".";
        }

        if (identifier.ends_with(".")) {
            identifier.pop_back();
        }

        if (!customizedType->getGenericArguments().empty()) {
            identifier += ".-";
            for (const auto& genericArgument : customizedType->getGenericArguments()) {
                identifier += genericArgument->generateInternalNamePart() + "-";
            }
            identifier.pop_back();
            identifier += "-.";
        }

        return identifier;
    }
} // namespace vnlc
