#ifndef VNLC_CONTROL_FLOW_STATEMENT_NODE_HPP
#define VNLC_CONTROL_FLOW_STATEMENT_NODE_HPP

#include "ast/statement/StatementNode.hpp"

namespace vnlc {
    class ControlFlowStatementNode : public StatementNode {
    protected:
        ControlFlowStatementNode(const Token& firstToken, const Token& lastToken) noexcept;

    public:
        ControlFlowStatementNode() = delete;
    };
} // namespace vnlc

#endif // VNLC_CONTROL_FLOW_STATEMENT_NODE_HPP