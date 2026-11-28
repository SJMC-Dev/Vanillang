#include "DeclarationNode.hpp"
#include "metadata/MetadataTerm.hpp"

namespace vnlc {
    DeclarationNode::DeclarationNode(AccessModifier accessModifier, std::unique_ptr<IdentifierNode>&& name, const Token& firstToken, const Token& lastToken) noexcept
        : AstNode(firstToken, lastToken),
          accessModifier(accessModifier),
          name(std::move(name)),
          includeMetadata(false),
          metadataTerms() {}

    DeclarationNode::DeclarationNode(
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<MetadataTerm>&& metadataTerms
    ) noexcept
        : AstNode(firstToken, lastToken),
          accessModifier(accessModifier),
          name(std::move(name)),
          includeMetadata(true),
          metadataTerms(std::move(metadataTerms)) {}

    AccessModifier DeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    const IdentifierNode& DeclarationNode::getName() const noexcept {
        return *name;
    }

    bool DeclarationNode::doesIncludeMetadata() const noexcept {
        return includeMetadata;
    }

    const std::vector<MetadataTerm>& DeclarationNode::getMetadataTerms() const noexcept {
        return metadataTerms;
    }

    ValueDeclarationNode::ValueDeclarationNode(
        ValueKind kind,
        Context context,
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::unique_ptr<TypeAnnotationNode>&& type,
        std::unique_ptr<ExpressionNode>&& initializer,
        bool includeTryKeyword,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : DeclarationNode(accessModifier, std::move(name), firstToken, lastToken),
          kind(kind),
          context(context),
          type(std::move(type)),
          initializer(std::move(initializer)),
          includeTryKeyword(includeTryKeyword) {}

    ValueDeclarationNode::ValueDeclarationNode(
        ValueKind kind,
        Context context,
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::unique_ptr<TypeAnnotationNode>&& type,
        std::unique_ptr<ExpressionNode>&& initializer,
        bool includeTryKeyword,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<MetadataTerm>&& metadataTerms
    ) noexcept
        : DeclarationNode(accessModifier, std::move(name), firstToken, lastToken, std::move(metadataTerms)),
          kind(kind),
          context(context),
          type(std::move(type)),
          initializer(std::move(initializer)),
          includeTryKeyword(includeTryKeyword) {}

    ValueKind ValueDeclarationNode::getKind() const noexcept {
        return kind;
    }

    Context ValueDeclarationNode::getContext() const noexcept {
        return context;
    }

    const TypeAnnotationNode* ValueDeclarationNode::getType() const noexcept {
        return type.get();
    }

    const ExpressionNode* ValueDeclarationNode::getInitializer() const noexcept {
        return initializer.get();
    }

    bool ValueDeclarationNode::doesIncludeTryKeyword() const noexcept {
        return includeTryKeyword;
    }

    FunctionDeclarationNode::FunctionDeclarationNode(
        FunctionImplementation implementation,
        Context context,
        AccessModifier accessModifier,
        FunctionReceiverKind binding,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
        std::unique_ptr<TypeAnnotationNode>&& returnType,
        std::unique_ptr<BlockStatementNode>&& body,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : DeclarationNode(accessModifier, std::move(name), firstToken, lastToken),
          implementation(implementation),
          context(context),
          binding(binding),
          parameters(std::move(parameters)),
          returnType(std::move(returnType)),
          body(std::move(body)),
          genericParameters(std::move(genericParameters)),
          genericConstraints(std::move(genericConstraints)) {}

    FunctionDeclarationNode::FunctionDeclarationNode(
        FunctionImplementation implementation,
        Context context,
        AccessModifier accessModifier,
        FunctionReceiverKind binding,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
        std::unique_ptr<TypeAnnotationNode>&& returnType,
        std::unique_ptr<BlockStatementNode>&& body,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<MetadataTerm>&& metadataTerms
    ) noexcept
        : DeclarationNode(accessModifier, std::move(name), firstToken, lastToken, std::move(metadataTerms)),
          implementation(implementation),
          context(context),
          binding(binding),
          parameters(std::move(parameters)),
          returnType(std::move(returnType)),
          body(std::move(body)),
          genericParameters(std::move(genericParameters)),
          genericConstraints(std::move(genericConstraints)) {}

    FunctionImplementation FunctionDeclarationNode::getImplementation() const noexcept {
        return implementation;
    }

    Context FunctionDeclarationNode::getContext() const noexcept {
        return context;
    }

    FunctionReceiverKind FunctionDeclarationNode::getBinding() const noexcept {
        return binding;
    }

    const std::vector<std::unique_ptr<ValueDeclarationNode>>& FunctionDeclarationNode::getParameters() const noexcept {
        return parameters;
    }

    const TypeAnnotationNode* FunctionDeclarationNode::getReturnType() const noexcept {
        return returnType.get();
    }

    const BlockStatementNode* FunctionDeclarationNode::getBody() const noexcept {
        return body.get();
    }

    const std::vector<std::unique_ptr<IdentifierNode>>& FunctionDeclarationNode::getGenericParameters() const noexcept {
        return genericParameters;
    }

    const std::vector<GenericConstraintTerm>& FunctionDeclarationNode::getGenericConstraints() const noexcept {
        return genericConstraints;
    }

    TypeDeclarationNode::TypeDeclarationNode(
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : DeclarationNode(accessModifier, std::move(name), firstToken, lastToken),
          genericParameters(std::move(genericParameters)),
          genericConstraints(std::move(genericConstraints)) {}

    TypeDeclarationNode::TypeDeclarationNode(
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<MetadataTerm>&& metadataTerms
    ) noexcept
        : DeclarationNode(accessModifier, std::move(name), firstToken, lastToken, std::move(metadataTerms)),
          genericParameters(std::move(genericParameters)),
          genericConstraints(std::move(genericConstraints)) {}

    const std::vector<std::unique_ptr<IdentifierNode>>& TypeDeclarationNode::getGenericParameters() const noexcept {
        return genericParameters;
    }

    const std::vector<GenericConstraintTerm>& TypeDeclarationNode::getGenericConstraints() const noexcept {
        return genericConstraints;
    }

    ClassDeclarationNode::ClassDeclarationNode(
        AccessModifier accessModifier,
        bool final,
        std::unique_ptr<IdentifierNode>&& name,
        std::unique_ptr<TypeAnnotationNode>&& baseClass,
        std::vector<std::unique_ptr<TypeAnnotationNode>>&& implementedInterfaces,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& properties,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& constructors,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& operators,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(std::move(genericParameters), std::move(genericConstraints), accessModifier, std::move(name), firstToken, lastToken),
          final(final),
          baseClass(std::move(baseClass)),
          implementedInterfaces(std::move(implementedInterfaces)),
          properties(std::move(properties)),
          methods(std::move(methods)),
          constructors(std::move(constructors)),
          operators(std::move(operators)) {}

    ClassDeclarationNode::ClassDeclarationNode(
        AccessModifier accessModifier,
        bool final,
        std::unique_ptr<IdentifierNode>&& name,
        std::unique_ptr<TypeAnnotationNode>&& baseClass,
        std::vector<std::unique_ptr<TypeAnnotationNode>>&& implementedInterfaces,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& properties,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& constructors,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& operators,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(std::move(genericParameters), std::move(genericConstraints), accessModifier, std::move(name), firstToken, lastToken, std::move(metadataTerms)),
          final(final),
          baseClass(std::move(baseClass)),
          implementedInterfaces(std::move(implementedInterfaces)),
          properties(std::move(properties)),
          methods(std::move(methods)),
          constructors(std::move(constructors)),
          operators(std::move(operators)) {}

    bool ClassDeclarationNode::isFinal() const noexcept {
        return final;
    }

    const TypeAnnotationNode* ClassDeclarationNode::getBaseClass() const noexcept {
        return baseClass.get();
    }

    const std::vector<std::unique_ptr<TypeAnnotationNode>>& ClassDeclarationNode::getImplementedInterfaces() const noexcept {
        return implementedInterfaces;
    }

    const std::vector<std::unique_ptr<ValueDeclarationNode>>& ClassDeclarationNode::getProperties() const noexcept {
        return properties;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& ClassDeclarationNode::getMethods() const noexcept {
        return methods;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& ClassDeclarationNode::getConstructors() const noexcept {
        return constructors;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& ClassDeclarationNode::getOperators() const noexcept {
        return operators;
    }

    InterfaceDeclarationNode::InterfaceDeclarationNode(
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& operators,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(std::move(genericParameters), std::move(genericConstraints), accessModifier, std::move(name), firstToken, lastToken),
          methods(std::move(methods)),
          operators(std::move(operators)) {}

    InterfaceDeclarationNode::InterfaceDeclarationNode(
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& operators,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(std::move(genericParameters), std::move(genericConstraints), accessModifier, std::move(name), firstToken, lastToken, std::move(metadataTerms)),
          methods(std::move(methods)),
          operators(std::move(operators)) {}

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& InterfaceDeclarationNode::getMethods() const noexcept {
        return methods;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& InterfaceDeclarationNode::getOperators() const noexcept {
        return operators;
    }

    EnumMemberNode::EnumMemberNode(
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& associatedValues,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode({}, {}, AccessModifier::PUBLIC, std::move(name), firstToken, lastToken),
          associatedValues(std::move(associatedValues)) {}

    EnumMemberNode::EnumMemberNode(
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& associatedValues,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode({}, {}, AccessModifier::PUBLIC, std::move(name), firstToken, lastToken, std::move(metadataTerms)),
          associatedValues(std::move(associatedValues)) {}

    const std::vector<std::unique_ptr<ValueDeclarationNode>>& EnumMemberNode::getAssociatedValues() const noexcept {
        return associatedValues;
    }

    EnumDeclarationNode::EnumDeclarationNode(
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<EnumMemberNode>>&& members,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& staticProperties,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& operators,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(std::move(genericParameters), std::move(genericConstraints), accessModifier, std::move(name), firstToken, lastToken),
          members(std::move(members)),
          staticProperties(std::move(staticProperties)),
          methods(std::move(methods)),
          operators(std::move(operators)) {}

    EnumDeclarationNode::EnumDeclarationNode(
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<EnumMemberNode>>&& members,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& staticProperties,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& operators,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(std::move(genericParameters), std::move(genericConstraints), accessModifier, std::move(name), firstToken, lastToken, std::move(metadataTerms)),
          members(std::move(members)),
          staticProperties(std::move(staticProperties)),
          methods(std::move(methods)),
          operators(std::move(operators)) {}

    const std::vector<std::unique_ptr<EnumMemberNode>>& EnumDeclarationNode::getMembers() const noexcept {
        return members;
    }

    const std::vector<std::unique_ptr<ValueDeclarationNode>>& EnumDeclarationNode::getStaticProperties() const noexcept {
        return staticProperties;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& EnumDeclarationNode::getMethods() const noexcept {
        return methods;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& EnumDeclarationNode::getOperators() const noexcept {
        return operators;
    }

    OpaqueDeclarationNode::OpaqueDeclarationNode(
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::unique_ptr<TypeAnnotationNode>&& baseOpaqueType,
        std::vector<std::unique_ptr<TypeAnnotationNode>>&& implementedInterfaces,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& operators,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(std::move(genericParameters), std::move(genericConstraints), accessModifier, std::move(name), firstToken, lastToken),
          baseOpaqueType(std::move(baseOpaqueType)),
          implementedInterfaces(std::move(implementedInterfaces)),
          methods(std::move(methods)),
          operators(std::move(operators)) {}

    OpaqueDeclarationNode::OpaqueDeclarationNode(
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::unique_ptr<TypeAnnotationNode>&& baseOpaqueType,
        std::vector<std::unique_ptr<TypeAnnotationNode>>&& implementedInterfaces,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
        std::vector<std::unique_ptr<FunctionDeclarationNode>>&& operators,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(std::move(genericParameters), std::move(genericConstraints), accessModifier, std::move(name), firstToken, lastToken, std::move(metadataTerms)),
          baseOpaqueType(std::move(baseOpaqueType)),
          implementedInterfaces(std::move(implementedInterfaces)),
          methods(std::move(methods)),
          operators(std::move(operators)) {}

    const TypeAnnotationNode* OpaqueDeclarationNode::getBaseOpaqueType() const noexcept {
        return baseOpaqueType.get();
    }

    const std::vector<std::unique_ptr<TypeAnnotationNode>>& OpaqueDeclarationNode::getImplementedInterfaces() const noexcept {
        return implementedInterfaces;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& OpaqueDeclarationNode::getMethods() const noexcept {
        return methods;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& OpaqueDeclarationNode::getOperators() const noexcept {
        return operators;
    }

    TypeAliasDeclarationNode::TypeAliasDeclarationNode(
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::unique_ptr<TypeAnnotationNode>&& originalType,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(std::move(genericParameters), std::move(genericConstraints), accessModifier, std::move(name), firstToken, lastToken),
          originalType(std::move(originalType)) {}

    TypeAliasDeclarationNode::TypeAliasDeclarationNode(
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::unique_ptr<TypeAnnotationNode>&& originalType,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(std::move(genericParameters), std::move(genericConstraints), accessModifier, std::move(name), firstToken, lastToken, std::move(metadataTerms)),
          originalType(std::move(originalType)) {}

    const TypeAnnotationNode& TypeAliasDeclarationNode::getOriginalType() const noexcept {
        return *originalType;
    }

    ImportDeclarationNode::ImportDeclarationNode(ImportPathTerm&& paths, const Token& firstToken, const Token& lastToken) noexcept
        : DeclarationNode(AccessModifier::PUBLIC, std::make_unique<IdentifierNode>("", firstToken, lastToken), firstToken, lastToken),
          paths(std::move(paths)) {}

    const ImportPathTerm& ImportDeclarationNode::getNamePartsListWithAliases() const noexcept {
        return paths;
    }
} // namespace vnlc
