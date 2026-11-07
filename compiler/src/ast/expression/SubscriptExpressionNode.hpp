#pragma once

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    class SubscriptExpressionNode : public ExpressionNode {
    private:
        std::unique_ptr<ExpressionNode> object;
        std::unique_ptr<ExpressionNode> index;

    public:
        SubscriptExpressionNode() = delete;

        SubscriptExpressionNode(std::unique_ptr<ExpressionNode>&& object, std::unique_ptr<ExpressionNode>&& index, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ExpressionNode& getObject() const noexcept;
        [[nodiscard]] const ExpressionNode& getIndex() const noexcept;
    };
} // namespace vnlc
