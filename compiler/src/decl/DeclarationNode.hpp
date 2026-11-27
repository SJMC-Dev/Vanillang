#pragma once

#include "ast/AstNode.hpp"
#include "expr/ExpressionNode.hpp"
#include "generic/GenericConstraintTerm.hpp"
#include "identifier/IdentifierNode.hpp"
#include "import/ImportPathTerm.hpp"
#include "metadata/MetadataTerm.hpp"
#include "prop/LanguageProperty.hpp"
#include "stmt/StatementNode.hpp"
#include "type/TypeAnnotationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class BlockStatementNode;

    class DeclarationNode : public AstNode {
    private:
        bool includeMetadata;
        std::vector<MetadataTerm> metadataTerms;

    protected:
        DeclarationNode(const Token& firstToken, const Token& lastToken);
        DeclarationNode(const Token& firstToken, const Token& lastToken, std::vector<MetadataTerm>&& metadataTerms) noexcept;

    public:
        DeclarationNode() = delete;

        [[nodiscard]] bool doesIncludeMetadata() const noexcept;
        [[nodiscard]] const std::vector<MetadataTerm>& getMetadataTerms() const noexcept;
    };

    class ValueDeclarationNode : public DeclarationNode {
    private:
        ValueKind kind;
        Context context;
        AccessModifier accessModifier;

        std::unique_ptr<IdentifierNode> name;
        std::unique_ptr<TypeAnnotationNode> type;    // nullptr if no type annotation
        std::unique_ptr<ExpressionNode> initializer; // nullptr if no initializer

        bool includeTryKeyword;

    public:
        ValueDeclarationNode() = delete;

        ValueDeclarationNode(
            ValueKind kind,
            Context context,
            AccessModifier accessModifier,
            std::unique_ptr<IdentifierNode>&& name,
            std::unique_ptr<TypeAnnotationNode>&& type,
            std::unique_ptr<ExpressionNode>&& initializer,
            bool includeTryKeyword,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        ValueDeclarationNode(
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
        ) noexcept;

        [[nodiscard]] ValueKind getKind() const noexcept;
        [[nodiscard]] Context getContext() const noexcept;
        [[nodiscard]] AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const TypeAnnotationNode* getType() const noexcept;
        [[nodiscard]] const ExpressionNode* getInitializer() const noexcept;
        [[nodiscard]] bool doesIncludeTryKeyword() const noexcept;
    };

    class FunctionDeclarationNode : public DeclarationNode {
    private:
        FunctionImplementation implementation;
        Context context;
        AccessModifier accessModifier;
        FunctionReceiverKind binding;

        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<ValueDeclarationNode>> parameters;
        std::unique_ptr<TypeAnnotationNode> returnType; // nullptr if the return type is omitted or declared as a constructor
        std::unique_ptr<BlockStatementNode> body;       // nullptr if native or declared in an interface

        std::vector<std::unique_ptr<IdentifierNode>> genericParameters;
        std::vector<GenericConstraintTerm> genericConstraints;

    public:
        FunctionDeclarationNode() = delete;

        FunctionDeclarationNode(
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
        ) noexcept;

        FunctionDeclarationNode(
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
        ) noexcept;

        [[nodiscard]] FunctionImplementation getImplementation() const noexcept;
        [[nodiscard]] Context getContext() const noexcept;
        [[nodiscard]] AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] FunctionReceiverKind getBinding() const noexcept;
        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ValueDeclarationNode>>& getParameters() const noexcept;
        [[nodiscard]] const TypeAnnotationNode* getReturnType() const noexcept;
        [[nodiscard]] const BlockStatementNode* getBody() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& getGenericParameters() const noexcept;
        [[nodiscard]] const std::vector<GenericConstraintTerm>& getGenericConstraints() const noexcept;
    };

    class TypeDeclarationNode : public DeclarationNode {
    protected:
        TypeDeclarationNode(const Token& firstToken, const Token& lastToken) noexcept;
        TypeDeclarationNode(const Token& firstToken, const Token& lastToken, std::vector<MetadataTerm>&& metadataTerms) noexcept;

    public:
        TypeDeclarationNode() = delete;
    };

    class ClassDeclarationNode : public TypeDeclarationNode {
    private:
        AccessModifier accessModifier;
        bool final;

        std::unique_ptr<IdentifierNode> name;
        std::unique_ptr<TypeAnnotationNode> baseClass;                          // nullptr if no base class
        std::vector<std::unique_ptr<TypeAnnotationNode>> implementedInterfaces; // empty if no implemented interfaces

        std::vector<std::unique_ptr<ValueDeclarationNode>> properties;
        std::vector<std::unique_ptr<FunctionDeclarationNode>> methods;
        std::vector<std::unique_ptr<FunctionDeclarationNode>> constructors;
        std::vector<std::unique_ptr<FunctionDeclarationNode>> operators;

        std::vector<std::unique_ptr<IdentifierNode>> genericParameters;
        std::vector<GenericConstraintTerm> genericConstraints;

    public:
        ClassDeclarationNode() = delete;

        ClassDeclarationNode(
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
        ) noexcept;

        ClassDeclarationNode(
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
        ) noexcept;

        [[nodiscard]] AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] bool isFinal() const noexcept;
        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const TypeAnnotationNode* getBaseClass() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<TypeAnnotationNode>>& getImplementedInterfaces() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ValueDeclarationNode>>& getProperties() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<FunctionDeclarationNode>>& getMethods() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<FunctionDeclarationNode>>& getConstructors() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<FunctionDeclarationNode>>& getOperators() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& getGenericParameters() const noexcept;
        [[nodiscard]] const std::vector<GenericConstraintTerm>& getGenericConstraints() const noexcept;
    };

    class InterfaceDeclarationNode : public TypeDeclarationNode {
    private:
        AccessModifier accessModifier;

        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<FunctionDeclarationNode>> methods;
        std::vector<std::unique_ptr<FunctionDeclarationNode>> operators;

        std::vector<std::unique_ptr<IdentifierNode>> genericParameters;
        std::vector<GenericConstraintTerm> genericConstraints;

    public:
        InterfaceDeclarationNode() = delete;

        InterfaceDeclarationNode(
            AccessModifier accessModifier,
            std::unique_ptr<IdentifierNode>&& name,
            std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
            std::vector<std::unique_ptr<FunctionDeclarationNode>>&& operators,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
            std::vector<GenericConstraintTerm>&& genericConstraints,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        InterfaceDeclarationNode(
            AccessModifier accessModifier,
            std::unique_ptr<IdentifierNode>&& name,
            std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methods,
            std::vector<std::unique_ptr<FunctionDeclarationNode>>&& operators,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
            std::vector<GenericConstraintTerm>&& genericConstraints,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<MetadataTerm>&& metadataTerms
        ) noexcept;

        [[nodiscard]] AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<FunctionDeclarationNode>>& getMethods() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<FunctionDeclarationNode>>& getOperators() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& getGenericParameters() const noexcept;
        [[nodiscard]] const std::vector<GenericConstraintTerm>& getGenericConstraints() const noexcept;
    };

    class EnumMemberDeclarationNode : public TypeDeclarationNode {
    private:
        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<ValueDeclarationNode>> associatedValues;

    public:
        EnumMemberDeclarationNode() = delete;

        EnumMemberDeclarationNode(
            std::unique_ptr<IdentifierNode>&& name,
            std::vector<std::unique_ptr<ValueDeclarationNode>>&& associatedValues,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        EnumMemberDeclarationNode(
            std::unique_ptr<IdentifierNode>&& name,
            std::vector<std::unique_ptr<ValueDeclarationNode>>&& associatedValues,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<MetadataTerm>&& metadataTerms
        ) noexcept;

        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ValueDeclarationNode>>& getAssociatedValues() const noexcept;
    };

    class EnumDeclarationNode : public TypeDeclarationNode {
    private:
        AccessModifier accessModifier;

        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<EnumMemberDeclarationNode>> memberDeclarations;

        std::vector<std::unique_ptr<IdentifierNode>> genericParameters;
        std::vector<GenericConstraintTerm> genericConstraints;

    public:
        EnumDeclarationNode() = delete;

        EnumDeclarationNode(
            AccessModifier accessModifier,
            std::unique_ptr<IdentifierNode>&& name,
            std::vector<std::unique_ptr<EnumMemberDeclarationNode>>&& memberDeclarations,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
            std::vector<GenericConstraintTerm>&& genericConstraints,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        EnumDeclarationNode(
            AccessModifier accessModifier,
            std::unique_ptr<IdentifierNode>&& name,
            std::vector<std::unique_ptr<EnumMemberDeclarationNode>>&& memberDeclarations,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
            std::vector<GenericConstraintTerm>&& genericConstraints,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<MetadataTerm>&& metadataTerms
        ) noexcept;

        [[nodiscard]] AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<EnumMemberDeclarationNode>>& getMemberDeclarations() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& getGenericParameters() const noexcept;
        [[nodiscard]] const std::vector<GenericConstraintTerm>& getGenericConstraints() const noexcept;
    };

    class OpaqueDeclarationNode : public TypeDeclarationNode {
    private:
        AccessModifier accessModifier;

        std::unique_ptr<IdentifierNode> name;
        std::unique_ptr<TypeAnnotationNode> baseOpaqueType;                     // nullptr if no base type
        std::vector<std::unique_ptr<TypeAnnotationNode>> implementedInterfaces; // empty if no implemented interfaces

        std::vector<std::unique_ptr<FunctionDeclarationNode>> methods;
        std::vector<std::unique_ptr<FunctionDeclarationNode>> operators;

        std::vector<std::unique_ptr<IdentifierNode>> genericParameters;
        std::vector<GenericConstraintTerm> genericConstraints;

    public:
        OpaqueDeclarationNode() = delete;

        OpaqueDeclarationNode(
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
        ) noexcept;

        OpaqueDeclarationNode(
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
        ) noexcept;

        [[nodiscard]] AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const TypeAnnotationNode* getBaseOpaqueType() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<TypeAnnotationNode>>& getImplementedInterfaces() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<FunctionDeclarationNode>>& getMethods() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<FunctionDeclarationNode>>& getOperators() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& getGenericParameters() const noexcept;
        [[nodiscard]] const std::vector<GenericConstraintTerm>& getGenericConstraints() const noexcept;
    };

    class TypeAliasDeclarationNode : public TypeDeclarationNode {
    private:
        AccessModifier accessModifier;

        std::unique_ptr<IdentifierNode> aliasName;
        std::unique_ptr<TypeAnnotationNode> originalType;

        std::vector<std::unique_ptr<IdentifierNode>> genericParameters;
        std::vector<GenericConstraintTerm> genericConstraints;

    public:
        TypeAliasDeclarationNode() = delete;

        TypeAliasDeclarationNode(
            AccessModifier accessModifier,
            std::unique_ptr<IdentifierNode>&& aliasName,
            std::unique_ptr<TypeAnnotationNode>&& originalType,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
            std::vector<GenericConstraintTerm>&& genericConstraints,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        TypeAliasDeclarationNode(
            AccessModifier accessModifier,
            std::unique_ptr<IdentifierNode>&& aliasName,
            std::unique_ptr<TypeAnnotationNode>&& originalType,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
            std::vector<GenericConstraintTerm>&& genericConstraints,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<MetadataTerm>&& metadataTerms
        ) noexcept;

        [[nodiscard]] AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] const IdentifierNode& getAliasName() const noexcept;
        [[nodiscard]] const TypeAnnotationNode& getOriginalType() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& getGenericParameters() const noexcept;
        [[nodiscard]] const std::vector<GenericConstraintTerm>& getGenericConstraints() const noexcept;
    };

    class ImportDeclarationNode : public DeclarationNode {
    private:
        ImportPathTerm paths;

    public:
        ImportDeclarationNode() = delete;

        ImportDeclarationNode(ImportPathTerm&& paths, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ImportPathTerm& getNamePartsListWithAliases() const noexcept;
    };
} // namespace vnlc
