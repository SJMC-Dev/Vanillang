#pragma once

#include "ast/expression/PrimaryExpressionNode.hpp"
#include "ast/expression/PrimitiveTypeExpressionKind.hpp"

namespace vnlc {
    class PrimitiveTypeExpressionNode : public PrimaryExpressionNode {
    private:
        PrimitiveTypeExpressionKind kind;

    public:
        PrimitiveTypeExpressionNode() = delete;

        PrimitiveTypeExpressionNode(PrimitiveTypeExpressionKind kind, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] PrimitiveTypeExpressionKind getKind() const noexcept;
    };
} // namespace vnlc
