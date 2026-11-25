#include "BreakStatementNode.hpp"
#include <memory>

namespace vnlc {
    BreakStatementNode::BreakStatementNode(const Token& firstToken, const Token& lastToken) noexcept : ControlFlowStatementNode(firstToken, lastToken), label(nullptr) {}

    BreakStatementNode::BreakStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          label(std::move(label)) {}

    const IdentifierNode* BreakStatementNode::getLabel() const noexcept {
        return label.get();
    }
} // namespace vnlc
