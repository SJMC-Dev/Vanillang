#pragma once

#include "ast/statement/ForStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct ForStatementParsingResult {
        std::unique_ptr<ForStatementNode> statement;
    };
} // namespace vnlc
