#pragma once

#include "ast/statement/StatementNode.hpp"

namespace vnlc {
    class ControlFlowStatementNode : public StatementNode {
    protected:
        ControlFlowStatementNode(const Token& firstToken, const Token& lastToken) noexcept;

    public:
        ControlFlowStatementNode() = delete;
    };
} // namespace vnlc
