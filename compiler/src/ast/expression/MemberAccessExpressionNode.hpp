#pragma once

#include "ast/expression/ExpressionNode.hpp"
#include "ast/expression/IdentifierLikeExpressionNode.hpp"
#include "ast/expression/MemberAccessExpressionKind.hpp"
#include <memory>

namespace vnlc {
    class MemberAccessExpressionNode : public ExpressionNode {
    private:
        MemberAccessExpressionKind kind;
        std::unique_ptr<ExpressionNode> object;
        std::unique_ptr<IdentifierLikeExpressionNode> member;

    public:
        MemberAccessExpressionNode() = delete;

        MemberAccessExpressionNode(
            MemberAccessExpressionKind kind,
            std::unique_ptr<ExpressionNode>&& object,
            std::unique_ptr<IdentifierLikeExpressionNode>&& member,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] MemberAccessExpressionKind getKind() const noexcept;
        [[nodiscard]] const ExpressionNode& getObject() const noexcept;
        [[nodiscard]] const IdentifierLikeExpressionNode& getMember() const noexcept;
    };
} // namespace vnlc
