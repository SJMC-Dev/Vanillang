#pragma once

#include "ast/AstNode.hpp"

namespace vnlc {
    class StatementNode : public AstNode {
    protected:
        StatementNode(const Token& firstToken, const Token& lastToken);

    public:
        StatementNode() = delete;
    };
} // namespace vnlc
