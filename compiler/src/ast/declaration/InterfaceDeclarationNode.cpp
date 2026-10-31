#include "InterfaceDeclarationNode.hpp"

namespace vnlc {
    InterfaceDeclarationNode::InterfaceDeclarationNode(
        TypeDeclarationKind::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methodDeclarations,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : InterfaceDeclarationNode(accessModifier, std::move(name), std::move(genericParameterNames), std::move(methodDeclarations), {}, firstToken, lastToken) {}

    InterfaceDeclarationNode::InterfaceDeclarationNode(
        TypeDeclarationKind::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methodDeclarations,
        std::vector<std::unique_ptr<OperatorDeclarationNode>>&& operatorDeclarations,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken),
          accessModifier(accessModifier),
          name(std::move(name)),
          genericParameterNames(std::move(genericParameterNames)),
          methodDeclarations(std::move(methodDeclarations)),
          operatorDeclarations(std::move(operatorDeclarations)) {}

    InterfaceDeclarationNode::InterfaceDeclarationNode(
        TypeDeclarationKind::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methodDeclarations,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : InterfaceDeclarationNode(accessModifier, std::move(name), std::move(genericParameterNames), std::move(methodDeclarations), {}, firstToken, lastToken, std::move(metadataTerms)) {}

    InterfaceDeclarationNode::InterfaceDeclarationNode(
        TypeDeclarationKind::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methodDeclarations,
        std::vector<std::unique_ptr<OperatorDeclarationNode>>&& operatorDeclarations,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          accessModifier(accessModifier),
          name(std::move(name)),
          genericParameterNames(std::move(genericParameterNames)),
          methodDeclarations(std::move(methodDeclarations)),
          operatorDeclarations(std::move(operatorDeclarations)) {}

    TypeDeclarationKind::AccessModifier InterfaceDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    const IdentifierNode& InterfaceDeclarationNode::getName() const noexcept {
        return *name;
    }

    const std::vector<std::unique_ptr<IdentifierNode>>& InterfaceDeclarationNode::getGenericParameterNames() const noexcept {
        return genericParameterNames;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& InterfaceDeclarationNode::getMethodDeclarations() const noexcept {
        return methodDeclarations;
    }

    const std::vector<std::unique_ptr<OperatorDeclarationNode>>& InterfaceDeclarationNode::getOperatorDeclarations() const noexcept {
        return operatorDeclarations;
    }
} // namespace vnlc
