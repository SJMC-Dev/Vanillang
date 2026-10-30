#ifndef VNLC_THIS_EXPRESSION_NODE_HPP
#define VNLC_THIS_EXPRESSION_NODE_HPP

#include "ast/expression/PrimaryExpressionNode.hpp"

namespace vnlc {
    class ThisExpressionNode : public PrimaryExpressionNode {
    public:
        ThisExpressionNode() = delete;

        ThisExpressionNode(const Token& firstToken, const Token& lastToken) noexcept;
    };
} // namespace vnlc

#endif // VNLC_THIS_EXPRESSION_NODE_HPP