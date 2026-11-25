#pragma once

#include "ast/expression/ExpressionNode.hpp"

namespace vnlc {
    class PrimaryExpressionNode : public ExpressionNode {
    protected:
        PrimaryExpressionNode(const Token& firstToken, const Token& lastToken) noexcept;

    public:
        PrimaryExpressionNode() = delete;
    };
} // namespace vnlc
