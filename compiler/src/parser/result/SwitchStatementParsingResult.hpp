#pragma once

#include "ast/statement/SwitchStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct SwitchStatementParsingResult {
        std::unique_ptr<SwitchStatementNode> statement;
    };
} // namespace vnlc
