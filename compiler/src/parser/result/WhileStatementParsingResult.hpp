#pragma once

#include "ast/statement/WhileStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct WhileStatementParsingResult {
        std::unique_ptr<WhileStatementNode> statement;
    };
} // namespace vnlc
