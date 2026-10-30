#ifndef VNLC_LOOP_STATEMENT_NODE_HPP
#define VNLC_LOOP_STATEMENT_NODE_HPP

#include "ast/identifier/IdentifierNode.hpp"
#include "ast/statement/ControlFlowStatementNode.hpp"
#include <memory>

namespace vnlc {
    class LoopStatementNode : public ControlFlowStatementNode {
    private:
        std::unique_ptr<IdentifierNode> label; // nullptr if no label

    protected:
        LoopStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept;

    public:
        LoopStatementNode() = delete;

        [[nodiscard]] const IdentifierNode* getLabel() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_LOOP_STATEMENT_NODE_HPP