#include "EnumDeclarationNode.hpp"
#include "ast/identifier/IdentifierNode.hpp"

namespace vnlc {
    EnumDeclarationNode::EnumDeclarationNode(
        TypeDeclarationKind::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<EnumMemberDeclarationNode>>&& memberDeclarations,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken),
          accessModifier(accessModifier),
          name(std::move(name)),
          genericParameterNames(std::move(genericParameterNames)),
          memberDeclarations(std::move(memberDeclarations)) {}

    EnumDeclarationNode::EnumDeclarationNode(
        TypeDeclarationKind::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<EnumMemberDeclarationNode>>&& memberDeclarations,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          accessModifier(accessModifier),
          name(std::move(name)),
          genericParameterNames(std::move(genericParameterNames)),
          memberDeclarations(std::move(memberDeclarations)) {}

    TypeDeclarationKind::AccessModifier EnumDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    const IdentifierNode& EnumDeclarationNode::getName() const noexcept {
        return *name;
    }

    const std::vector<std::unique_ptr<EnumMemberDeclarationNode>>& EnumDeclarationNode::getMemberDeclarations() const noexcept {
        return memberDeclarations;
    }

    const std::vector<std::unique_ptr<IdentifierNode>>& EnumDeclarationNode::getGenericParameterNames() const noexcept {
        return genericParameterNames;
    }
} // namespace vnlc
