#pragma once

#include "outline/TypeDeclarationOutline.hpp"
#include <string>
#include <string_view>

namespace vnlc {
    class ClassDeclarationOutline : public TypeDeclarationOutline {
    private:
        std::string name;

    public:
        ClassDeclarationOutline() = delete;

        ClassDeclarationOutline(std::string&& name, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] std::string_view getName() const noexcept;
    };
} // namespace vnlc
