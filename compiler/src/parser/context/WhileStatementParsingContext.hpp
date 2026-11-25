#pragma once

#include "ast/identifier/IdentifierNode.hpp"
#include <memory>

namespace vnlc {
    struct WhileStatementParsingContext {
        std::unique_ptr<IdentifierNode> label; // nullptr if no label
    };
} // namespace vnlc
