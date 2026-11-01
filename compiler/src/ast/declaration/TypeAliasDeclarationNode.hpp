#ifndef VNLC_TYPE_ALIAS_DECLARATION_NODE_HPP
#define VNLC_TYPE_ALIAS_DECLARATION_NODE_HPP

#include "ast/declaration/TypeDeclarationKind.hpp"
#include "ast/declaration/TypeDeclarationNode.hpp"
#include "ast/typeannotation/TypeAnnotationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class TypeAliasDeclarationNode : public TypeDeclarationNode {
    private:
        TypeDeclarationKind::AccessModifier accessModifier;
        std::unique_ptr<IdentifierNode> aliasName;
        std::vector<std::unique_ptr<IdentifierNode>> genericParameterNames;
        std::unique_ptr<TypeAnnotationNode> originalType;

    public:
        TypeAliasDeclarationNode() = delete;

        TypeAliasDeclarationNode(
            TypeDeclarationKind::AccessModifier accessModifier,
            std::unique_ptr<IdentifierNode>&& aliasName,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
            std::unique_ptr<TypeAnnotationNode>&& originalType,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        TypeAliasDeclarationNode(
            TypeDeclarationKind::AccessModifier accessModifier,
            std::unique_ptr<IdentifierNode>&& aliasName,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
            std::unique_ptr<TypeAnnotationNode>&& originalType,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
        ) noexcept;

        [[nodiscard]] TypeDeclarationKind::AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] const IdentifierNode& getAliasName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& getGenericParameterNames() const noexcept;
        [[nodiscard]] const TypeAnnotationNode& getOriginalType() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_TYPE_ALIAS_DECLARATION_NODE_HPP