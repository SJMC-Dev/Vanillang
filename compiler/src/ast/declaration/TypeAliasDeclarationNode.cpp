#include "TypeAliasDeclarationNode.hpp"

namespace vnlc {
    TypeAliasDeclarationNode::TypeAliasDeclarationNode(
        TypeDeclarationKind::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& aliasName,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::unique_ptr<TypeAnnotationNode>&& originalType,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken),
          accessModifier(accessModifier),
          aliasName(std::move(aliasName)),
          genericParameterNames(std::move(genericParameterNames)),
          originalType(std::move(originalType)) {}

    TypeAliasDeclarationNode::TypeAliasDeclarationNode(
        TypeDeclarationKind::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& aliasName,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::unique_ptr<TypeAnnotationNode>&& originalType,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          accessModifier(accessModifier),
          aliasName(std::move(aliasName)),
          genericParameterNames(std::move(genericParameterNames)),
          originalType(std::move(originalType)) {}

    TypeDeclarationKind::AccessModifier TypeAliasDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    const IdentifierNode& TypeAliasDeclarationNode::getAliasName() const noexcept {
        return *aliasName;
    }

    const std::vector<std::unique_ptr<IdentifierNode>>& TypeAliasDeclarationNode::getGenericParameterNames() const noexcept {
        return genericParameterNames;
    }

    const TypeAnnotationNode& TypeAliasDeclarationNode::getOriginalType() const noexcept {
        return *originalType;
    }
} // namespace vnlc
