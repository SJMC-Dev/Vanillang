#pragma once

#include "ast/expression/PrimaryExpressionNode.hpp"

namespace vnlc {
    class ThisExpressionNode : public PrimaryExpressionNode {
    public:
        ThisExpressionNode() = delete;

        ThisExpressionNode(const Token& firstToken, const Token& lastToken) noexcept;
    };
} // namespace vnlc
