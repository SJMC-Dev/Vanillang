#pragma once

#include "outline/DeclarationOutline.hpp"

namespace vnlc {
    class TypeDeclarationOutline : public DeclarationOutline {
    protected:
        TypeDeclarationOutline(const Token& firstToken, const Token& lastToken) noexcept;

    public:
        TypeDeclarationOutline() = delete;
    };
} // namespace vnlc
