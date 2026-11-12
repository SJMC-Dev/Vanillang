#pragma once

#include "ast/expression/ExpressionNode.hpp"
#include <memory>
#include <string>

namespace vnlc {
    struct DictEntryParsingResult {
        std::string key;
        std::unique_ptr<ExpressionNode> value;
    };
} // namespace vnlc
