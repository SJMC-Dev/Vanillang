#ifndef VNLC_TYPE_ALIAS_DECLARATION_OUTLINE_HPP
#define VNLC_TYPE_ALIAS_DECLARATION_OUTLINE_HPP

#include "outline/TypeDeclarationOutline.hpp"
#include <string>
#include <string_view>

namespace vnlc {
    class TypeAliasDeclarationOutline : public TypeDeclarationOutline {
    private:
        std::string aliasName;

    public:
        TypeAliasDeclarationOutline() = delete;

        TypeAliasDeclarationOutline(std::string&& aliasName, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] std::string_view getAliasName() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_TYPE_ALIAS_DECLARATION_OUTLINE_HPP
