#ifndef VNLC_EXPRESSION_NODE_HPP
#define VNLC_EXPRESSION_NODE_HPP

#include "ast/AstNode.hpp"

namespace vnlc {
    class ExpressionNode : public AstNode {
    protected:
        ExpressionNode(const Token& firstToken, const Token& lastToken);

    public:
        ExpressionNode() = delete;
    };
} // namespace vnlc

#endif // VNLC_EXPRESSION_NODE_HPP