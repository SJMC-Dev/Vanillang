#pragma once

#include "ast/expression/ExpressionNode.hpp"
#include "ast/expression/UnaryExpressionKind.hpp"
#include <memory>

namespace vnlc {
    class UnaryExpressionNode : public ExpressionNode {
    private:
        UnaryExpressionKind kind;
        std::unique_ptr<ExpressionNode> operand;

    public:
        UnaryExpressionNode() = delete;

        UnaryExpressionNode(UnaryExpressionKind kind, std::unique_ptr<ExpressionNode>&& operand, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] UnaryExpressionKind getKind() const noexcept;
        [[nodiscard]] const ExpressionNode& getOperand() const noexcept;
    };
} // namespace vnlc
