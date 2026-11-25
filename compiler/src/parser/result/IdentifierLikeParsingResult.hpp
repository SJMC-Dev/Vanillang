#pragma once

#include "ast/expression/IdentifierLikeExpressionNode.hpp"
#include <memory>

namespace vnlc {
    struct IdentifierLikeParsingResult {
        std::unique_ptr<IdentifierLikeExpressionNode> expression;
    };
} // namespace vnlc
