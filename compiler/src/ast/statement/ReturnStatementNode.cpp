#include "ReturnStatementNode.hpp"

namespace vnlc {
    ReturnStatementNode::ReturnStatementNode(const Token& firstToken, const Token& lastToken) noexcept : ControlFlowStatementNode(firstToken, lastToken), returnValue(nullptr) {}

    ReturnStatementNode::ReturnStatementNode(std::unique_ptr<ExpressionNode>&& returnValue, const Token& firstToken, const Token& lastToken) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          returnValue(std::move(returnValue)) {}

    const ExpressionNode* ReturnStatementNode::getReturnValue() const noexcept {
        return returnValue.get();
    }
} // namespace vnlc
