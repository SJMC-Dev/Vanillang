#pragma once

#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/statement/StatementNode.hpp"
#include <memory>

namespace vnlc {
    class VariableDeclarationStatementNode : public StatementNode {
    private:
        std::unique_ptr<ValueDeclarationNode> variableDeclaration;

    public:
        VariableDeclarationStatementNode() = delete;

        VariableDeclarationStatementNode(std::unique_ptr<ValueDeclarationNode>&& variableDeclaration, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ValueDeclarationNode& getVariableDeclaration() const noexcept;
    };
} // namespace vnlc
