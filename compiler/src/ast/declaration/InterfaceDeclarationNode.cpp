#include "InterfaceDeclarationNode.hpp"

namespace vnlc {
    InterfaceDeclarationNode::InterfaceDeclarationNode(
        TypeDeclarationKind::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : InterfaceDeclarationNode(accessModifier, std::move(name), std::move(genericParameterNames), std::move(methods), {}, firstToken, lastToken) {}

    InterfaceDeclarationNode::InterfaceDeclarationNode(
        TypeDeclarationKind::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
        std::vector<std::unique_ptr<OperatorDeclarationNode>>&& operators,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken),
          accessModifier(accessModifier),
          name(std::move(name)),
          genericParameterNames(std::move(genericParameterNames)),
          methods(std::move(methods)),
          operators(std::move(operators)) {}

    InterfaceDeclarationNode::InterfaceDeclarationNode(
        TypeDeclarationKind::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : InterfaceDeclarationNode(accessModifier, std::move(name), std::move(genericParameterNames), std::move(methods), {}, firstToken, lastToken, std::move(metadataTerms)) {}

    InterfaceDeclarationNode::InterfaceDeclarationNode(
        TypeDeclarationKind::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
        std::vector<std::unique_ptr<OperatorDeclarationNode>>&& operators,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          accessModifier(accessModifier),
          name(std::move(name)),
          genericParameterNames(std::move(genericParameterNames)),
          methods(std::move(methods)),
          operators(std::move(operators)) {}

    TypeDeclarationKind::AccessModifier InterfaceDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    const IdentifierNode& InterfaceDeclarationNode::getName() const noexcept {
        return *name;
    }

    const std::vector<std::unique_ptr<IdentifierNode>>& InterfaceDeclarationNode::getGenericParameterNames() const noexcept {
        return genericParameterNames;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& InterfaceDeclarationNode::getMethods() const noexcept {
        return methods;
    }

    const std::vector<std::unique_ptr<OperatorDeclarationNode>>& InterfaceDeclarationNode::getOperatorDeclarations() const noexcept {
        return operators;
    }
} // namespace vnlc
