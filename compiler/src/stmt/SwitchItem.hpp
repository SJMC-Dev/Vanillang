#pragma once

#include "expr/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    class StatementNode;

    struct LiteralMatchItem {
        std::unique_ptr<ExpressionNode> literal;
        std::unique_ptr<StatementNode> body;
    };

    struct TypeMatchItem {
        std::unique_ptr<TypeAnnotationNode> type;
        std::unique_ptr<StatementNode> body;
        std::unique_ptr<ExpressionNode> guardExpression; // nullptr if no guard expression
    };
} // namespace vnlc