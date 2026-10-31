#ifndef VNLC_STATEMENT_NODE_HPP
#define VNLC_STATEMENT_NODE_HPP

#include "ast/AstNode.hpp"

namespace vnlc {
    class StatementNode : public AstNode {
    protected:
        StatementNode(const Token& firstToken, const Token& lastToken);

    public:
        StatementNode() = delete;
    };
} // namespace vnlc

#endif // VNLC_STATEMENT_NODE_HPP