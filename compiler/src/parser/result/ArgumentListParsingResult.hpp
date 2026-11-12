#pragma once

#include "ast/expression/ExpressionNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct ArgumentListParsingResult {
        std::vector<std::unique_ptr<ExpressionNode>> arguments;
        std::unique_ptr<ExpressionNode> context; // nullptr if no context argument
    };
} // namespace vnlc
