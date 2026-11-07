#ifndef VNLC_SELECTOR_LITERAL_EXPRESSION_ITEM_HPP
#define VNLC_SELECTOR_LITERAL_EXPRESSION_ITEM_HPP

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

#endif // VNLC_SELECTOR_LITERAL_EXPRESSION_ITEM_HPP
