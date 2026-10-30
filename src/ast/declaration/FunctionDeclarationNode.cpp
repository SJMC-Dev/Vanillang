#include "FunctionDeclarationNode.hpp"

namespace vnlc {
    FunctionDeclarationNode::FunctionDeclarationNode(
        FunctionDeclarationKind::Implementation implementation,
        FunctionDeclarationKind::Context context,
        FunctionDeclarationKind::AccessModifier accessModifier,
        FunctionDeclarationKind::Binding binding,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
        std::optional<std::unique_ptr<TypeReferenceNode>>&& returnType,
        std::optional<std::unique_ptr<BlockStatementNode>>&& body,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : DeclarationNode(firstToken, lastToken),
          implementation(implementation),
          context(context),
          accessModifier(accessModifier),
          binding(binding),
          name(std::move(name)),
          parameters(std::move(parameters)),
          returnType(std::move(returnType)),
          body(std::move(body)),
          internalName("") {}

    FunctionDeclarationNode::FunctionDeclarationNode(
        FunctionDeclarationKind::Implementation implementation,
        FunctionDeclarationKind::Context context,
        FunctionDeclarationKind::AccessModifier accessModifier,
        FunctionDeclarationKind::Binding binding,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
        std::optional<std::unique_ptr<TypeReferenceNode>>&& returnType,
        std::optional<std::unique_ptr<BlockStatementNode>>&& body,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : DeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          implementation(implementation),
          context(context),
          accessModifier(accessModifier),
          binding(binding),
          name(std::move(name)),
          parameters(std::move(parameters)),
          returnType(std::move(returnType)),
          body(std::move(body)),
          internalName("") {}

    FunctionDeclarationKind::Implementation FunctionDeclarationNode::getImplementation() const noexcept {
        return implementation;
    }

    FunctionDeclarationKind::Context FunctionDeclarationNode::getContext() const noexcept {
        return context;
    }

    FunctionDeclarationKind::AccessModifier FunctionDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    FunctionDeclarationKind::Binding FunctionDeclarationNode::getBinding() const noexcept {
        return binding;
    }

    const IdentifierNode& FunctionDeclarationNode::getName() const noexcept {
        return *name;
    }

    std::string_view FunctionDeclarationNode::getInternalName() const noexcept {
        return internalName;
    }

    const std::vector<std::unique_ptr<ValueDeclarationNode>>& FunctionDeclarationNode::getParameters() const noexcept {
        return parameters;
    }

    const std::optional<std::unique_ptr<TypeReferenceNode>>& FunctionDeclarationNode::getReturnType() const noexcept {
        return returnType;
    }

    const std::optional<std::unique_ptr<BlockStatementNode>>& FunctionDeclarationNode::getBody() const noexcept {
        return body;
    }

    void FunctionDeclarationNode::setInternalName(std::string_view internalName) const {
        this->internalName = std::string(internalName);
    }
} // namespace vnlc
