#pragma once

#include "ast/statement/ControlFlowStatementNode.hpp"

namespace vnlc {
    class ReloadStatementNode : public ControlFlowStatementNode {
    public:
        ReloadStatementNode() = delete;

        ReloadStatementNode(const Token& firstToken, const Token& lastToken) noexcept;
    };
} // namespace vnlc
