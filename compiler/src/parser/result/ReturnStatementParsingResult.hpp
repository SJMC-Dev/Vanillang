#pragma once

#include "ast/statement/ReturnStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct ReturnStatementParsingResult {
        std::unique_ptr<ReturnStatementNode> statement;
    };
} // namespace vnlc
