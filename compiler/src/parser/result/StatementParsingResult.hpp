#pragma once

#include "ast/statement/StatementNode.hpp"
#include <memory>

namespace vnlc {
    struct StatementParsingResult {
        std::unique_ptr<StatementNode> statement;
    };
} // namespace vnlc
