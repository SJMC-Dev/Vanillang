#pragma once

#include "ast/statement/ExpressionStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct ExpressionStatementParsingResult {
        std::unique_ptr<ExpressionStatementNode> statement;
    };
} // namespace vnlc
