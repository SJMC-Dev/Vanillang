#ifndef VNLC_SIMPLE_LITERAL_EXPRESSION_NODE_HPP
#define VNLC_SIMPLE_LITERAL_EXPRESSION_NODE_HPP

#include "ast/expression/LiteralExpressionNode.hpp"
#include "ast/expression/SimpleLiteralExpressionKind.hpp"

namespace vnlc {
    class SimpleLiteralExpressionNode : public LiteralExpressionNode {
    private:
        SimpleLiteralExpressionKind kind;
        std::string literal;

    public:
        SimpleLiteralExpressionNode() = delete;

        SimpleLiteralExpressionNode(SimpleLiteralExpressionKind kind, std::string_view literal, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] SimpleLiteralExpressionKind getKind() const noexcept;
        [[nodiscard]] std::string_view getLiteral() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_SIMPLE_LITERAL_EXPRESSION_NODE_HPP