#ifndef VNLC_SWITCH_STATEMENT_NODE_HPP
#define VNLC_SWITCH_STATEMENT_NODE_HPP

#include "ast/statement/ControlFlowStatementNode.hpp"
#include "ast/statement/SwitchStatementItem.hpp"
#include "ast/statement/SwitchStatementKind.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class SwitchStatementNode : public ControlFlowStatementNode {
    private:
        std::unique_ptr<ExpressionNode> switchExpression;
        SwitchStatementKind switchKind;
        std::vector<SwitchStatementItem::LiteralMatchItem> literalMatchItems; // empty if switchKind is TYPE_MATCH
        std::vector<SwitchStatementItem::TypeMatchItem> typeMatchItems;       // empty if switchKind is LITERAL_MATCH
        std::unique_ptr<StatementNode> defaultCaseBody;                       // nullptr if no default case

    public:
        SwitchStatementNode() = delete;

        SwitchStatementNode(
            std::unique_ptr<ExpressionNode>&& switchExpression,
            std::vector<SwitchStatementItem::LiteralMatchItem>&& literalMatchItems,
            std::unique_ptr<StatementNode>&& defaultCaseBody,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        SwitchStatementNode(
            std::unique_ptr<ExpressionNode>&& switchExpression,
            std::vector<SwitchStatementItem::TypeMatchItem>&& typeMatchItems,
            std::unique_ptr<StatementNode>&& defaultCaseBody,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const ExpressionNode& getSwitchExpression() const noexcept;
        [[nodiscard]] SwitchStatementKind getSwitchKind() const noexcept;
        [[nodiscard]] const std::vector<SwitchStatementItem::LiteralMatchItem>& getLiteralMatchItems() const noexcept;
        [[nodiscard]] const std::vector<SwitchStatementItem::TypeMatchItem>& getTypeMatchItems() const noexcept;
        [[nodiscard]] const StatementNode* getDefaultCaseBody() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_SWITCH_STATEMENT_NODE_HPP