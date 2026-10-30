#ifndef VNLC_TYPE_DECLARATION_OUTLINE_HPP
#define VNLC_TYPE_DECLARATION_OUTLINE_HPP

#include "outline/DeclarationOutline.hpp"

namespace vnlc {
    class TypeDeclarationOutline : public DeclarationOutline {
    protected:
        TypeDeclarationOutline(const Token& firstToken, const Token& lastToken) noexcept;

    public:
        TypeDeclarationOutline() = delete;
    };
} // namespace vnlc

#endif // VNLC_TYPE_DECLARATION_OUTLINE_HPP
