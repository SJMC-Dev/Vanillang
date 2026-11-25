#include "SelectorLiteralExpressionNode.hpp"

namespace vnlc {
    SelectorLiteralExpressionNode::SelectorLiteralExpressionNode(
        SelectorLiteralExpressionKind kind,
        std::vector<SelectorLiteralExpressionItem::Argument>&& arguments,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LiteralExpressionNode(firstToken, lastToken),
          kind(kind),
          arguments(std::move(arguments)) {}

    SelectorLiteralExpressionKind SelectorLiteralExpressionNode::getKind() const noexcept {
        return kind;
    }

    const std::vector<SelectorLiteralExpressionItem::Argument>& SelectorLiteralExpressionNode::getArguments() const noexcept {
        return arguments;
    }
} // namespace vnlc
