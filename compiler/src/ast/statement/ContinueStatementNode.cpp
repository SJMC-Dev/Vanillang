#include "ContinueStatementNode.hpp"

namespace vnlc {
    ContinueStatementNode::ContinueStatementNode(const Token& firstToken, const Token& lastToken) noexcept : ControlFlowStatementNode(firstToken, lastToken), label(nullptr) {}

    ContinueStatementNode::ContinueStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          label(std::move(label)) {}

    const IdentifierNode* ContinueStatementNode::getLabel() const noexcept {
        return label.get();
    }
} // namespace vnlc
