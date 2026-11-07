#pragma once

#include "ast/statement/BreakStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct BreakStatementParsingResult {
        std::unique_ptr<BreakStatementNode> statement;
    };
} // namespace vnlc
