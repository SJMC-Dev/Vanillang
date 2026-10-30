#ifndef VNLC_OPERATOR_DECLARATION_NODE_HPP
#define VNLC_OPERATOR_DECLARATION_NODE_HPP

#include "ast/declaration/DeclarationNode.hpp"
#include "ast/declaration/OperatorDeclarationKind.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/statement/BlockStatementNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace vnlc {
    class OperatorDeclarationNode : public DeclarationNode {
    private:
        OperatorDeclarationKind::Kind kind;
        OperatorDeclarationKind::Implementation implementation;
        OperatorDeclarationKind::Context context;
        OperatorDeclarationKind::AccessModifier accessModifier;
        std::vector<std::unique_ptr<ValueDeclarationNode>> parameters;
        std::unique_ptr<TypeReferenceNode> returnType; // nullptr if the return type is omitted
        std::unique_ptr<BlockStatementNode> body;      // nullptr if native or declared in an interface
        mutable std::string internalName;

    public:
        OperatorDeclarationNode() = delete;

        OperatorDeclarationNode(
            OperatorDeclarationKind::Kind kind,
            OperatorDeclarationKind::Implementation implementation,
            OperatorDeclarationKind::Context context,
            OperatorDeclarationKind::AccessModifier accessModifier,
            std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
            std::unique_ptr<TypeReferenceNode>&& returnType,
            std::unique_ptr<BlockStatementNode>&& body,
            const Token& firstToken,
            const Token& lastToken
        );

        OperatorDeclarationNode(
            OperatorDeclarationKind::Kind kind,
            OperatorDeclarationKind::Implementation implementation,
            OperatorDeclarationKind::Context context,
            OperatorDeclarationKind::AccessModifier accessModifier,
            std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
            std::unique_ptr<TypeReferenceNode>&& returnType,
            std::unique_ptr<BlockStatementNode>&& body,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
        );

        [[nodiscard]] OperatorDeclarationKind::Kind getKind() const noexcept;
        [[nodiscard]] OperatorDeclarationKind::Implementation getImplementation() const noexcept;
        [[nodiscard]] OperatorDeclarationKind::Context getContext() const noexcept;
        [[nodiscard]] OperatorDeclarationKind::AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] std::string_view getInternalName() const noexcept;
        [[nodiscard]] std::string_view getOperatorName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ValueDeclarationNode>>& getParameters() const noexcept;
        [[nodiscard]] const TypeReferenceNode* getReturnType() const noexcept;
        [[nodiscard]] const BlockStatementNode* getBody() const noexcept;

        void setInternalName(std::string_view internalName) const;
    };
} // namespace vnlc

#endif // VNLC_OPERATOR_DECLARATION_NODE_HPP
