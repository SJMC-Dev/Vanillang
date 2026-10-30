#ifndef VNLC_PRIMARY_EXPRESSION_NODE_HPP
#define VNLC_PRIMARY_EXPRESSION_NODE_HPP

#include "ast/expression/ExpressionNode.hpp"

namespace vnlc {
    class PrimaryExpressionNode : public ExpressionNode {
    protected:
        PrimaryExpressionNode(const Token& firstToken, const Token& lastToken) noexcept;

    public:
        PrimaryExpressionNode() = delete;
    };
} // namespace vnlc

#endif // VNLC_PRIMARY_EXPRESSION_NODE_HPP
