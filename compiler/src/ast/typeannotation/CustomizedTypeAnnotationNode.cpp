#include "CustomizedTypeAnnotationNode.hpp"

namespace vnlc {
    CustomizedTypeAnnotationNode::CustomizedTypeAnnotationNode(
        bool questionMarkSuffix,
        std::vector<std::unique_ptr<IdentifierNode>>&& nameParts,
        std::vector<std::unique_ptr<TypeAnnotationNode>>&& genericArguments,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeAnnotationNode(questionMarkSuffix, firstToken, lastToken),
          nameParts(std::move(nameParts)),
          genericArguments(std::move(genericArguments)) {}

    const std::vector<std::unique_ptr<IdentifierNode>>& CustomizedTypeAnnotationNode::getNameParts() const noexcept {
        return nameParts;
    }

    const std::vector<std::unique_ptr<TypeAnnotationNode>>& CustomizedTypeAnnotationNode::getGenericArguments() const noexcept {
        return genericArguments;
    }
} // namespace vnlc
