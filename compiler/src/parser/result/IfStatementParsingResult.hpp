#pragma once

#include "ast/statement/IfStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct IfStatementParsingResult {
        std::unique_ptr<IfStatementNode> statement;
    };
} // namespace vnlc
