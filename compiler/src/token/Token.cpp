#include "Token.hpp"

namespace vnlc {
    Token::Token(TokenKind kind, std::string_view value, std::size_t line, std::size_t column, std::size_t offset) : kind(kind), value(value), offset(offset), line(line), column(column) {}

    TokenKind Token::getKind() const noexcept {
        return kind;
    }

    std::size_t Token::getLine() const noexcept {
        return line;
    }

    std::size_t Token::getColumn() const noexcept {
        return column;
    }

    std::size_t Token::getOffset() const noexcept {
        return offset;
    }

    std::size_t Token::getLength() const noexcept {
        return static_cast<std::size_t>(value.length());
    }

    std::string_view Token::getValue() const noexcept {
        return value;
    }
} // namespace vnlc
