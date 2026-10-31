#ifndef VNLC_TYPE_REFERENCE_NODE_HPP
#define VNLC_TYPE_REFERENCE_NODE_HPP

#include "ast/AstNode.hpp"
#include <string>

namespace vnlc {
    class TypeReferenceNode : public AstNode {
    private:
        bool questionMarkSuffix;

    protected:
        TypeReferenceNode(bool questionMarkSuffix, const Token& firstToken, const Token& lastToken) noexcept;

    public:
        TypeReferenceNode() = delete;

        [[nodiscard]] bool hasQuestionMarkSuffix() const noexcept;
        [[nodiscard]] std::string generateInternalNamePart() const;
    };
} // namespace vnlc

#endif // VNLC_TYPE_REFERENCE_NODE_HPP
