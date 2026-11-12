#include "ClassDeclarationNode.hpp"

namespace vnlc {
    ClassDeclarationNode::ClassDeclarationNode(
        TypeDeclarationKind::AccessModifier accessModifier,
        bool final,
        std::unique_ptr<IdentifierNode>&& name,
        std::unique_ptr<TypeAnnotationNode>&& baseClass,
        std::vector<std::unique_ptr<TypeAnnotationNode>>&& implementedInterfaces,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& properties,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
        std::vector<std::unique_ptr<ConstructorDeclarationNode>>&& constructors,
        std::vector<std::unique_ptr<OperatorDeclarationNode>>&& operators,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken),
          accessModifier(accessModifier),
          final(final),
          name(std::move(name)),
          baseClass(std::move(baseClass)),
          implementedInterfaces(std::move(implementedInterfaces)),
          genericParameterNames(std::move(genericParameterNames)),
          properties(std::move(properties)),
          methods(std::move(methods)),
          constructors(std::move(constructors)),
          operators(std::move(operators)) {}

    ClassDeclarationNode::ClassDeclarationNode(
        TypeDeclarationKind::AccessModifier accessModifier,
        bool final,
        std::unique_ptr<IdentifierNode>&& name,
        std::unique_ptr<TypeAnnotationNode>&& baseClass,
        std::vector<std::unique_ptr<TypeAnnotationNode>>&& implementedInterfaces,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& properties,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
        std::vector<std::unique_ptr<ConstructorDeclarationNode>>&& constructors,
        std::vector<std::unique_ptr<OperatorDeclarationNode>>&& operators,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          accessModifier(accessModifier),
          final(final),
          name(std::move(name)),
          baseClass(std::move(baseClass)),
          implementedInterfaces(std::move(implementedInterfaces)),
          genericParameterNames(std::move(genericParameterNames)),
          properties(std::move(properties)),
          methods(std::move(methods)),
          constructors(std::move(constructors)),
          operators(std::move(operators)) {}

    TypeDeclarationKind::AccessModifier ClassDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    bool ClassDeclarationNode::isFinal() const noexcept {
        return final;
    }

    const IdentifierNode& ClassDeclarationNode::getName() const noexcept {
        return *name;
    }

    const TypeAnnotationNode* ClassDeclarationNode::getBaseClass() const noexcept {
        return baseClass.get();
    }

    const std::vector<std::unique_ptr<TypeAnnotationNode>>& ClassDeclarationNode::getImplementedInterfaces() const noexcept {
        return implementedInterfaces;
    }

    const std::vector<std::unique_ptr<IdentifierNode>>& ClassDeclarationNode::getGenericParameterNames() const noexcept {
        return genericParameterNames;
    }

    const std::vector<std::unique_ptr<ValueDeclarationNode>>& ClassDeclarationNode::getProperties() const noexcept {
        return properties;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& ClassDeclarationNode::getMethods() const noexcept {
        return methods;
    }

    const std::vector<std::unique_ptr<ConstructorDeclarationNode>>& ClassDeclarationNode::getConstructorDeclarations() const noexcept {
        return constructors;
    }

    const std::vector<std::unique_ptr<OperatorDeclarationNode>>& ClassDeclarationNode::getOperatorDeclarations() const noexcept {
        return operators;
    }
} // namespace vnlc
