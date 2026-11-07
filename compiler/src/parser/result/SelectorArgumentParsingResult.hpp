#pragma once

#include "ast/expression/ExpressionNode.hpp"
#include <memory>
#include <string>

namespace vnlc {
    struct SelectorArgumentParsingResult {
        std::string key;
        bool negated;
        std::unique_ptr<ExpressionNode> value;
    };
} // namespace vnlc
