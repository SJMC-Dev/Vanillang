#ifndef VNLC_CLASS_DECLARATION_NODE_HPP
#define VNLC_CLASS_DECLARATION_NODE_HPP

#include "ast/declaration/ConstructorDeclarationNode.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/OperatorDeclarationNode.hpp"
#include "ast/declaration/TypeDeclarationKind.hpp"
#include "ast/declaration/TypeDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/identifier/IdentifierNode.hpp"
#include "ast/typeannotation/TypeAnnotationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class ClassDeclarationNode : public TypeDeclarationNode {
    private:
        TypeDeclarationKind::AccessModifier accessModifier;
        bool final;
        std::unique_ptr<IdentifierNode> name;
        std::unique_ptr<TypeAnnotationNode> baseClass;                          // nullptr if no base class
        std::vector<std::unique_ptr<TypeAnnotationNode>> implementedInterfaces; // empty if no implemented interfaces
        std::vector<std::unique_ptr<IdentifierNode>> genericParameterNames;
        std::vector<std::unique_ptr<ValueDeclarationNode>> properties; // instance or static properties only
        std::vector<std::unique_ptr<FunctionDeclarationNode>> methods;
        std::vector<std::unique_ptr<ConstructorDeclarationNode>> constructors;
        std::vector<std::unique_ptr<OperatorDeclarationNode>> operators;

    public:
        ClassDeclarationNode() = delete;

        ClassDeclarationNode(
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
        ) noexcept;

        ClassDeclarationNode(
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
        ) noexcept;

        [[nodiscard]] TypeDeclarationKind::AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] bool isFinal() const noexcept;
        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const TypeAnnotationNode* getBaseClass() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<TypeAnnotationNode>>& getImplementedInterfaces() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& getGenericParameterNames() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ValueDeclarationNode>>& getProperties() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<FunctionDeclarationNode>>& getMethods() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ConstructorDeclarationNode>>& getConstructorDeclarations() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<OperatorDeclarationNode>>& getOperatorDeclarations() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_CLASS_DECLARATION_NODE_HPP
