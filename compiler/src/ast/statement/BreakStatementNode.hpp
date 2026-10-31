#ifndef VNLC_BREAK_STATEMENT_NODE_HPP
#define VNLC_BREAK_STATEMENT_NODE_HPP

#include "ast/identifier/IdentifierNode.hpp"
#include "ast/statement/ControlFlowStatementNode.hpp"
#include <memory>

namespace vnlc {
    class BreakStatementNode : public ControlFlowStatementNode {
    private:
        std::unique_ptr<IdentifierNode> label; // nullptr if no label

    public:
        BreakStatementNode() = delete;

        BreakStatementNode(const Token& firstToken, const Token& lastToken) noexcept;
        BreakStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const IdentifierNode* getLabel() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_BREAK_STATEMENT_NODE_HPP