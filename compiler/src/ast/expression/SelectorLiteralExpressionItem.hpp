#pragma once

#include "ast/expression/ExpressionNode.hpp"
#include <memory>
#include <string>

namespace vnlc {
    namespace SelectorLiteralExpressionItem {
        struct Argument {
            std::string key;
            bool negated;
            std::unique_ptr<ExpressionNode> value;
        };
    }; // namespace SelectorLiteralExpressionItem
} // namespace vnlc
