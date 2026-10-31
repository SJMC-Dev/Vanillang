#ifndef VNLC_TOKEN_HPP
#define VNLC_TOKEN_HPP

#include "token/TokenKind.hpp"

#include <string>
#include <string_view>

namespace vnlc {
    class Token {
    private:
        TokenKind kind;
        std::string value;
        std::size_t offset;
        std::size_t line;
        std::size_t column;

    public:
        Token(TokenKind kind, std::string_view value, std::size_t line, std::size_t column, std::size_t offset);

        Token(const Token& other) = default;
        Token(Token&& other) noexcept = default;

        Token& operator=(const Token& other) = default;
        Token& operator=(Token&& other) noexcept = default;

        [[nodiscard]] TokenKind getKind() const noexcept;
        [[nodiscard]] std::size_t getLine() const noexcept;
        [[nodiscard]] std::size_t getColumn() const noexcept;
        [[nodiscard]] std::size_t getOffset() const noexcept;
        [[nodiscard]] std::size_t getLength() const noexcept;
        [[nodiscard]] std::string_view getValue() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_TOKEN_HPP
