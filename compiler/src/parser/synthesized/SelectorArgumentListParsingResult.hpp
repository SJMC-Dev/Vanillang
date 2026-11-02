#ifndef VNLC_SELECTOR_ARGUMENT_LIST_PARSING_RESULT_HPP
#define VNLC_SELECTOR_ARGUMENT_LIST_PARSING_RESULT_HPP

#include "ast/expression/SelectorLiteralExpressionItem.hpp"
#include <vector>

namespace vnlc {
    struct SelectorArgumentListParsingResult {
        std::vector<SelectorLiteralExpressionItem::Argument> arguments;
    };
} // namespace vnlc

#endif // VNLC_SELECTOR_ARGUMENT_LIST_PARSING_RESULT_HPP
