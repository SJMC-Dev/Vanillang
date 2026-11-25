#pragma once

#include "ast/expression/ExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct LiteralParsingResult {
        std::unique_ptr<ExpressionNode> expression;
    };
} // namespace vnlc
