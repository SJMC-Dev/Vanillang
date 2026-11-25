#pragma once

#include "ast/statement/BlockStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct BlockStatementParsingResult {
        std::unique_ptr<BlockStatementNode> statement;
    };
} // namespace vnlc
