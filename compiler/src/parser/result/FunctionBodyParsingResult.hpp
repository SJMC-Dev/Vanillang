#pragma once

#include "ast/statement/BlockStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct FunctionBodyParsingResult {
        std::unique_ptr<BlockStatementNode> body;
    };
} // namespace vnlc
