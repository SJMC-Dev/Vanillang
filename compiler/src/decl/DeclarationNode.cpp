#include "DeclarationNode.hpp"
#include "metadata/MetadataTerm.hpp"

namespace vnlc {
    DeclarationNode::DeclarationNode(const Token& firstToken, const Token& lastToken) : AstNode(firstToken, lastToken), includeMetadata(false), metadataTerms() {}

    DeclarationNode::DeclarationNode(const Token& firstToken, const Token& lastToken, std::vector<MetadataTerm>&& metadataTerms) noexcept
        : AstNode(firstToken, lastToken),
          includeMetadata(true),
          metadataTerms(std::move(metadataTerms)) {}

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
        : DeclarationNode(firstToken, lastToken),
          kind(kind),
          context(context),
          accessModifier(accessModifier),
          name(std::move(name)),
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
        : DeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          kind(kind),
          context(context),
          accessModifier(accessModifier),
          name(std::move(name)),
          type(std::move(type)),
          initializer(std::move(initializer)),
          includeTryKeyword(includeTryKeyword) {}

    ValueKind ValueDeclarationNode::getKind() const noexcept {
        return kind;
    }

    Context ValueDeclarationNode::getContext() const noexcept {
        return context;
    }

    AccessModifier ValueDeclarationNode::getAccessModifier() const noexcept {
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
        : DeclarationNode(firstToken, lastToken),
          implementation(implementation),
          context(context),
          accessModifier(accessModifier),
          binding(binding),
          name(std::move(name)),
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
        : DeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          implementation(implementation),
          context(context),
          accessModifier(accessModifier),
          binding(binding),
          name(std::move(name)),
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

    AccessModifier FunctionDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    FunctionReceiverKind FunctionDeclarationNode::getBinding() const noexcept {
        return binding;
    }

    const IdentifierNode& FunctionDeclarationNode::getName() const noexcept {
        return *name;
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

    TypeDeclarationNode::TypeDeclarationNode(const Token& firstToken, const Token& lastToken) noexcept : DeclarationNode(firstToken, lastToken) {}

    TypeDeclarationNode::TypeDeclarationNode(const Token& firstToken, const Token& lastToken, std::vector<MetadataTerm>&& metadataTerms) noexcept
        : DeclarationNode(firstToken, lastToken, std::move(metadataTerms)) {}

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
        : TypeDeclarationNode(firstToken, lastToken),
          accessModifier(accessModifier),
          final(final),
          name(std::move(name)),
          baseClass(std::move(baseClass)),
          implementedInterfaces(std::move(implementedInterfaces)),
          properties(std::move(properties)),
          methods(std::move(methods)),
          constructors(std::move(constructors)),
          operators(std::move(operators)),
          genericParameters(std::move(genericParameters)),
          genericConstraints(std::move(genericConstraints)) {}

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
        : TypeDeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          accessModifier(accessModifier),
          final(final),
          name(std::move(name)),
          baseClass(std::move(baseClass)),
          implementedInterfaces(std::move(implementedInterfaces)),
          properties(std::move(properties)),
          methods(std::move(methods)),
          constructors(std::move(constructors)),
          operators(std::move(operators)),
          genericParameters(std::move(genericParameters)),
          genericConstraints(std::move(genericConstraints)) {}

    AccessModifier ClassDeclarationNode::getAccessModifier() const noexcept {
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

    const std::vector<std::unique_ptr<IdentifierNode>>& ClassDeclarationNode::getGenericParameters() const noexcept {
        return genericParameters;
    }

    const std::vector<GenericConstraintTerm>& ClassDeclarationNode::getGenericConstraints() const noexcept {
        return genericConstraints;
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
        : TypeDeclarationNode(firstToken, lastToken),
          accessModifier(accessModifier),
          name(std::move(name)),
          methods(std::move(methods)),
          operators(std::move(operators)),
          genericParameters(std::move(genericParameters)),
          genericConstraints(std::move(genericConstraints)) {}

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
        : TypeDeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          accessModifier(accessModifier),
          name(std::move(name)),
          methods(std::move(methods)),
          operators(std::move(operators)),
          genericParameters(std::move(genericParameters)),
          genericConstraints(std::move(genericConstraints)) {}

    AccessModifier InterfaceDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    const IdentifierNode& InterfaceDeclarationNode::getName() const noexcept {
        return *name;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& InterfaceDeclarationNode::getMethods() const noexcept {
        return methods;
    }

    const std::vector<std::unique_ptr<FunctionDeclarationNode>>& InterfaceDeclarationNode::getOperators() const noexcept {
        return operators;
    }

    const std::vector<std::unique_ptr<IdentifierNode>>& InterfaceDeclarationNode::getGenericParameters() const noexcept {
        return genericParameters;
    }

    const std::vector<GenericConstraintTerm>& InterfaceDeclarationNode::getGenericConstraints() const noexcept {
        return genericConstraints;
    }

    EnumMemberDeclarationNode::EnumMemberDeclarationNode(
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& associatedValues,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken),
          name(std::move(name)),
          associatedValues(std::move(associatedValues)) {}

    EnumMemberDeclarationNode::EnumMemberDeclarationNode(
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& associatedValues,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          name(std::move(name)),
          associatedValues(std::move(associatedValues)) {}

    const IdentifierNode& EnumMemberDeclarationNode::getName() const noexcept {
        return *name;
    }

    const std::vector<std::unique_ptr<ValueDeclarationNode>>& EnumMemberDeclarationNode::getAssociatedValues() const noexcept {
        return associatedValues;
    }

    EnumDeclarationNode::EnumDeclarationNode(
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<EnumMemberDeclarationNode>>&& memberDeclarations,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken),
          accessModifier(accessModifier),
          name(std::move(name)),
          memberDeclarations(std::move(memberDeclarations)),
          genericParameters(std::move(genericParameters)),
          genericConstraints(std::move(genericConstraints)) {}

    EnumDeclarationNode::EnumDeclarationNode(
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<EnumMemberDeclarationNode>>&& memberDeclarations,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          accessModifier(accessModifier),
          name(std::move(name)),
          memberDeclarations(std::move(memberDeclarations)),
          genericParameters(std::move(genericParameters)),
          genericConstraints(std::move(genericConstraints)) {}

    AccessModifier EnumDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    const IdentifierNode& EnumDeclarationNode::getName() const noexcept {
        return *name;
    }

    const std::vector<std::unique_ptr<EnumMemberDeclarationNode>>& EnumDeclarationNode::getMemberDeclarations() const noexcept {
        return memberDeclarations;
    }

    const std::vector<std::unique_ptr<IdentifierNode>>& EnumDeclarationNode::getGenericParameters() const noexcept {
        return genericParameters;
    }

    const std::vector<GenericConstraintTerm>& EnumDeclarationNode::getGenericConstraints() const noexcept {
        return genericConstraints;
    }

    TypeAliasDeclarationNode::TypeAliasDeclarationNode(
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& aliasName,
        std::unique_ptr<TypeAnnotationNode>&& originalType,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken),
          accessModifier(accessModifier),
          aliasName(std::move(aliasName)),
          originalType(std::move(originalType)),
          genericParameters(std::move(genericParameters)),
          genericConstraints(std::move(genericConstraints)) {}

    TypeAliasDeclarationNode::TypeAliasDeclarationNode(
        AccessModifier accessModifier,
        std::unique_ptr<IdentifierNode>&& aliasName,
        std::unique_ptr<TypeAnnotationNode>&& originalType,
        std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
        std::vector<GenericConstraintTerm>&& genericConstraints,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<MetadataTerm>&& metadataTerms
    ) noexcept
        : TypeDeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          accessModifier(accessModifier),
          aliasName(std::move(aliasName)),
          originalType(std::move(originalType)),
          genericParameters(std::move(genericParameters)),
          genericConstraints(std::move(genericConstraints)) {}

    AccessModifier TypeAliasDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    const IdentifierNode& TypeAliasDeclarationNode::getAliasName() const noexcept {
        return *aliasName;
    }

    const TypeAnnotationNode& TypeAliasDeclarationNode::getOriginalType() const noexcept {
        return *originalType;
    }

    const std::vector<std::unique_ptr<IdentifierNode>>& TypeAliasDeclarationNode::getGenericParameters() const noexcept {
        return genericParameters;
    }

    const std::vector<GenericConstraintTerm>& TypeAliasDeclarationNode::getGenericConstraints() const noexcept {
        return genericConstraints;
    }

    ImportDeclarationNode::ImportDeclarationNode(ImportPathTerm&& paths, const Token& firstToken, const Token& lastToken) noexcept
        : DeclarationNode(firstToken, lastToken),
          paths(std::move(paths)) {}

    const ImportPathTerm& ImportDeclarationNode::getNamePartsListWithAliases() const noexcept {
        return paths;
    }
} // namespace vnlc
