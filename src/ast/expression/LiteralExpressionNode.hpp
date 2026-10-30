#ifndef VNLC_LITERAL_EXPRESSION_NODE_HPP
#define VNLC_LITERAL_EXPRESSION_NODE_HPP

#include "ast/expression/PrimaryExpressionNode.hpp"

namespace vnlc {
    class LiteralExpressionNode : public PrimaryExpressionNode {
    protected:
        LiteralExpressionNode(const Token& firstToken, const Token& lastToken) noexcept;

    public:
        LiteralExpressionNode() = delete;
    };
} // namespace vnlc

#endif // VNLC_LITERAL_EXPRESSION_NODE_HPP
