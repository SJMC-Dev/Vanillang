#pragma once

#include "ast/expression/PrimaryExpressionNode.hpp"

namespace vnlc {
    class NoneExpressionNode : public PrimaryExpressionNode {
    public:
        NoneExpressionNode() = delete;

        NoneExpressionNode(const Token& firstToken, const Token& lastToken) noexcept;
    };
} // namespace vnlc
