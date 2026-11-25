#include "ValueDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationKind.hpp"

namespace vnlc {
    ValueDeclarationNode::ValueDeclarationNode(
        ValueDeclarationKind::Kind kind,
        ValueDeclarationKind::Context context,
        ValueDeclarationKind::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::unique_ptr<TypeAnnotationNode>&& type,
        std::unique_ptr<ExpressionNode>&& initializer,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : DeclarationNode(firstToken, lastToken),
          kind(kind),
          context(context),
          accessModifier(accessModifier),
          name(std::move(name)),
          type(std::move(type)),
          initializer(std::move(initializer)) {}

    ValueDeclarationNode::ValueDeclarationNode(
        ValueDeclarationKind::Kind kind,
        ValueDeclarationKind::Context context,
        ValueDeclarationKind::AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::unique_ptr<TypeAnnotationNode>&& type,
        std::unique_ptr<ExpressionNode>&& initializer,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : DeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          kind(kind),
          context(context),
          accessModifier(accessModifier),
          name(std::move(name)),
          type(std::move(type)),
          initializer(std::move(initializer)) {}

    ValueDeclarationKind::Kind ValueDeclarationNode::getKind() const noexcept {
        return kind;
    }

    ValueDeclarationKind::Context ValueDeclarationNode::getContext() const noexcept {
        return context;
    }

    ValueDeclarationKind::AccessModifier ValueDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    const IdentifierNode& ValueDeclarationNode::getName() const noexcept {
        return *name;
    }

    const TypeAnnotationNode* ValueDeclarationNode::getType() const noexcept {
        return type.get();
    }

    const ExpressionNode* ValueDeclarationNode::getInitializer() const noexcept {
        return initializer.get();
    }
} // namespace vnlc
