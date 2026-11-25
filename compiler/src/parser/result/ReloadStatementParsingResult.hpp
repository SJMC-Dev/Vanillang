#pragma once

#include "ast/statement/ReloadStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct ReloadStatementParsingResult {
        std::unique_ptr<ReloadStatementNode> statement;
    };
} // namespace vnlc
