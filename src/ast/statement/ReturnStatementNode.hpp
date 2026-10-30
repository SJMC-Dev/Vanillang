#ifndef VNLC_RETURN_STATEMENT_NODE_HPP
#define VNLC_RETURN_STATEMENT_NODE_HPP

#include "ast/expression/ExpressionNode.hpp"
#include "ast/statement/ControlFlowStatementNode.hpp"
#include <memory>

namespace vnlc {
    class ReturnStatementNode : public ControlFlowStatementNode {
    private:
        std::unique_ptr<ExpressionNode> returnValue; // nullptr if no return value

    public:
        ReturnStatementNode() = delete;

        ReturnStatementNode(const Token& firstToken, const Token& lastToken) noexcept;
        ReturnStatementNode(std::unique_ptr<ExpressionNode>&& returnValue, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ExpressionNode* getReturnValue() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_RETURN_STATEMENT_NODE_HPP