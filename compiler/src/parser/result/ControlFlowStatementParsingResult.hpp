#pragma once

#include "ast/statement/ControlFlowStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct ControlFlowStatementParsingResult {
        std::unique_ptr<ControlFlowStatementNode> statement;
    };
} // namespace vnlc
