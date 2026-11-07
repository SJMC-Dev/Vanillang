#pragma once

#include "ast/identifier/IdentifierNode.hpp"
#include "ast/statement/ControlFlowStatementNode.hpp"
#include <memory>

namespace vnlc {
    class ContinueStatementNode : public ControlFlowStatementNode {
    private:
        std::unique_ptr<IdentifierNode> label; // nullptr if no label

    public:
        ContinueStatementNode() = delete;

        ContinueStatementNode(const Token& firstToken, const Token& lastToken) noexcept;
        ContinueStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const IdentifierNode* getLabel() const noexcept;
    };
} // namespace vnlc
