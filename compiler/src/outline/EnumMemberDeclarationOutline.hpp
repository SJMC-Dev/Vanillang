#pragma once

#include "outline/TypeDeclarationOutline.hpp"
#include <string>
#include <string_view>

namespace vnlc {
    class EnumDeclarationOutline;

    class EnumMemberDeclarationOutline : public TypeDeclarationOutline {
    private:
        std::string name;
        const EnumDeclarationOutline* enumDeclaration;

        friend class EnumDeclarationOutline;

    public:
        EnumMemberDeclarationOutline() = delete;

        EnumMemberDeclarationOutline(std::string&& name, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] std::string_view getName() const noexcept;
        [[nodiscard]] const EnumDeclarationOutline* getEnumDeclaration() const noexcept;
    };
} // namespace vnlc
