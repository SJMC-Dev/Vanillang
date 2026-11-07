#pragma once

#include "ast/expression/LiteralExpressionNode.hpp"
#include "ast/expression/SelectorLiteralExpressionItem.hpp"
#include "ast/expression/SelectorLiteralExpressionKind.hpp"
#include <vector>

namespace vnlc {
    class SelectorLiteralExpressionNode : public LiteralExpressionNode {
    private:
        SelectorLiteralExpressionKind kind;
        std::vector<SelectorLiteralExpressionItem::Argument> arguments;

    public:
        SelectorLiteralExpressionNode() = delete;

        SelectorLiteralExpressionNode(
            SelectorLiteralExpressionKind kind,
            std::vector<SelectorLiteralExpressionItem::Argument>&& arguments,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] SelectorLiteralExpressionKind getKind() const noexcept;
        [[nodiscard]] const std::vector<SelectorLiteralExpressionItem::Argument>& getArguments() const noexcept;
    };
} // namespace vnlc
