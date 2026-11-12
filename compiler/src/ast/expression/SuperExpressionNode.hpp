#pragma once

#include "ast/expression/PrimaryExpressionNode.hpp"

namespace vnlc {
    class SuperExpressionNode : public PrimaryExpressionNode {
    public:
        SuperExpressionNode() = delete;

        SuperExpressionNode(const Token& firstToken, const Token& lastToken) noexcept;
    };
} // namespace vnlc
