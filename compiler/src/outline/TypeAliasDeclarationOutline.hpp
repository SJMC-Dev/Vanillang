#pragma once

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
