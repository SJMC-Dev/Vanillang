#include "RangeExpressionNode.hpp"

namespace vnlc {
    RangeExpressionNode::RangeExpressionNode(std::unique_ptr<ExpressionNode>&& start, std::unique_ptr<ExpressionNode>&& end, const Token& firstToken, const Token& lastToken) noexcept
        : ExpressionNode(firstToken, lastToken),
          start(std::move(start)),
          end(std::move(end)) {}

    const ExpressionNode* RangeExpressionNode::getStart() const noexcept {
        return start.get();
    }

    const ExpressionNode* RangeExpressionNode::getEnd() const noexcept {
        return end.get();
    }
} // namespace vnlc
