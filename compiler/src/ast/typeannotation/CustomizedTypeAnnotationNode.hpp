#ifndef VNLC_CUSTOMIZED_TYPE_ANNOTATION_NODE_HPP
#define VNLC_CUSTOMIZED_TYPE_ANNOTATION_NODE_HPP

#include "ast/identifier/IdentifierNode.hpp"
#include "ast/typeannotation/TypeAnnotationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class CustomizedTypeAnnotationNode : public TypeAnnotationNode {
    private:
        std::vector<std::unique_ptr<IdentifierNode>> nameParts;
        std::vector<std::unique_ptr<TypeAnnotationNode>> genericArguments;

    public:
        CustomizedTypeAnnotationNode() = delete;

        CustomizedTypeAnnotationNode(
            bool questionMarkSuffix,
            std::vector<std::unique_ptr<IdentifierNode>>&& nameParts,
            std::vector<std::unique_ptr<TypeAnnotationNode>>&& genericArguments,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& getNameParts() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<TypeAnnotationNode>>& getGenericArguments() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_CUSTOMIZED_TYPE_ANNOTATION_NODE_HPP
