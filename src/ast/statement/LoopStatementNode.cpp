#include "LoopStatementNode.hpp"

namespace vnlc {
    LoopStatementNode::LoopStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          label(std::move(label)) {}

    const IdentifierNode* LoopStatementNode::getLabel() const noexcept {
        return label.get();
    }
} // namespace vnlc
