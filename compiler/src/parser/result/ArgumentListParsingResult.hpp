#ifndef VNLC_ARGUMENT_LIST_PARSING_RESULT_HPP
#define VNLC_ARGUMENT_LIST_PARSING_RESULT_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct ArgumentListParsingResult {
        std::vector<std::unique_ptr<ExpressionNode>> arguments;
        std::unique_ptr<ExpressionNode> context; // nullptr if no context argument
    };
} // namespace vnlc

#endif // VNLC_ARGUMENT_LIST_PARSING_RESULT_HPP
