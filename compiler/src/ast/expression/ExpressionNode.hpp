#pragma once

#include "ast/AstNode.hpp"

namespace vnlc {
    class ExpressionNode : public AstNode {
    protected:
        ExpressionNode(const Token& firstToken, const Token& lastToken);

    public:
        ExpressionNode() = delete;
    };
} // namespace vnlc
