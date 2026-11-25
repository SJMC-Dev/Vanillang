#pragma once

#include "ast/expression/SelectorLiteralExpressionItem.hpp"
#include <vector>

namespace vnlc {
    struct SelectorArgumentListParsingResult {
        std::vector<SelectorLiteralExpressionItem::Argument> arguments;
    };
} // namespace vnlc
