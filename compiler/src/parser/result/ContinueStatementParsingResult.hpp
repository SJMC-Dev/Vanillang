#pragma once

#include "ast/statement/ContinueStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct ContinueStatementParsingResult {
        std::unique_ptr<ContinueStatementNode> statement;
    };
} // namespace vnlc
