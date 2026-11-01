#ifndef VNLC_SWITCH_STATEMENT_ITEM_HPP
#define VNLC_SWITCH_STATEMENT_ITEM_HPP

#include "ast/expression/ExpressionNode.hpp"
#include "ast/statement/StatementNode.hpp"
#include "ast/typeannotation/TypeAnnotationNode.hpp"
#include <memory>

namespace vnlc {
    namespace SwitchStatementItem {
        struct LiteralMatchItem {
            std::unique_ptr<ExpressionNode> literal;
            std::unique_ptr<StatementNode> body;
        };

        struct TypeMatchItem {
            std::unique_ptr<TypeAnnotationNode> type;
            std::unique_ptr<StatementNode> body;
            std::unique_ptr<ExpressionNode> guardExpression; // nullptr if no guard expression
        };
    }; // namespace SwitchStatementItem
} // namespace vnlc

#endif // VNLC_SWITCH_STATEMENT_ITEM_HPP