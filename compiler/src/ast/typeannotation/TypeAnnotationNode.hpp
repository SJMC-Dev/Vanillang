#pragma once

#include "ast/AstNode.hpp"
#include <string>

namespace vnlc {
    class TypeAnnotationNode : public AstNode {
    private:
        bool questionMarkSuffix;

    protected:
        TypeAnnotationNode(bool questionMarkSuffix, const Token& firstToken, const Token& lastToken) noexcept;

    public:
        TypeAnnotationNode() = delete;

        [[nodiscard]] bool hasQuestionMarkSuffix() const noexcept;
        [[nodiscard]] std::string generateInternalNamePart() const;
    };
} // namespace vnlc
