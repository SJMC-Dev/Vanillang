#pragma once

#include <memory>
#include <string>

namespace vnlc {
    class ExpressionNode;

    struct SelectorArgumentTerm {
        std::string key;
        bool negated;
        std::unique_ptr<ExpressionNode> value;
    };
} // namespace vnlc