#ifndef VNLC_NONE_EXPRESSION_NODE_HPP
#define VNLC_NONE_EXPRESSION_NODE_HPP

#include "ast/expression/PrimaryExpressionNode.hpp"

namespace vnlc {
    class NoneExpressionNode : public PrimaryExpressionNode {
    public:
        NoneExpressionNode() = delete;

        NoneExpressionNode(const Token& firstToken, const Token& lastToken) noexcept;
    };
} // namespace vnlc

#endif // VNLC_NONE_EXPRESSION_NODE_HPP
