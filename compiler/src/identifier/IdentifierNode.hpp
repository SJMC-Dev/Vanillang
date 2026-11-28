#pragma once

#include "ast/AstNode.hpp"
#include <string>
#include <string_view>

namespace vnlc {
    class IdentifierNode : public AstNode { // also includes generalized identifier if needed
    private:
        std::string identifierString;

    public:
        IdentifierNode() = delete;

        IdentifierNode(std::string_view identifierString, const Token& firstToken, const Token& lastToken);

        [[nodiscard]] std::string_view getIdentifierString() const;
    };
} // namespace vnlc
