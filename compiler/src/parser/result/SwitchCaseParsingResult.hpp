#pragma once

#include "ast/expression/ExpressionNode.hpp"
#include "ast/statement/SwitchStatementKind.hpp"
#include "ast/typeannotation/TypeAnnotationNode.hpp"
#include <memory>

namespace vnlc {
    struct SwitchCaseParsingResult {
        SwitchStatementKind kind;
        std::unique_ptr<ExpressionNode> literal;         // nullptr if kind is TYPE_MATCH
        std::unique_ptr<TypeAnnotationNode> type;        // nullptr if kind is LITERAL_MATCH
        std::unique_ptr<ExpressionNode> guardExpression; // nullptr if no guard expression
    };
} // namespace vnlc
