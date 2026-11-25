#pragma once

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    class RangeExpressionNode : public ExpressionNode {
    private:
        std::unique_ptr<ExpressionNode> start; // nullptr if no start bound
        std::unique_ptr<ExpressionNode> end;   // nullptr if no end bound

    public:
        RangeExpressionNode() = delete;

        RangeExpressionNode(std::unique_ptr<ExpressionNode>&& start, std::unique_ptr<ExpressionNode>&& end, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ExpressionNode* getStart() const noexcept;
        [[nodiscard]] const ExpressionNode* getEnd() const noexcept;
    };
} // namespace vnlc
