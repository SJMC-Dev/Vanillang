#pragma once

#include "ast/expression/BinaryExpressionKind.hpp"
#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    class BinaryExpressionNode : public ExpressionNode {
    private:
        BinaryExpressionKind kind;
        std::unique_ptr<ExpressionNode> left;
        std::unique_ptr<ExpressionNode> right;

    public:
        BinaryExpressionNode() = delete;

        BinaryExpressionNode(
            BinaryExpressionKind kind,
            std::unique_ptr<ExpressionNode>&& left,
            std::unique_ptr<ExpressionNode>&& right,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] BinaryExpressionKind getKind() const noexcept;
        [[nodiscard]] const ExpressionNode& getLeft() const noexcept;
        [[nodiscard]] const ExpressionNode& getRight() const noexcept;
    };
} // namespace vnlc
