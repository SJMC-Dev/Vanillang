#pragma once

#include "ast/expression/PrimaryExpressionNode.hpp"

namespace vnlc {
    class LiteralExpressionNode : public PrimaryExpressionNode {
    protected:
        LiteralExpressionNode(const Token& firstToken, const Token& lastToken) noexcept;

    public:
        LiteralExpressionNode() = delete;
    };
} // namespace vnlc
