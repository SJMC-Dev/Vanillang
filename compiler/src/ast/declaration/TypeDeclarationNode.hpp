#pragma once

#include "ast/declaration/DeclarationNode.hpp"

namespace vnlc {
    class TypeDeclarationNode : public DeclarationNode {
    protected:
        TypeDeclarationNode(const Token& firstToken, const Token& lastToken) noexcept;
        TypeDeclarationNode(const Token& firstToken, const Token& lastToken, std::vector<DeclarationItem::MetadataTerm>&& metadataTerms) noexcept;

    public:
        TypeDeclarationNode() = delete;
    };
} // namespace vnlc
