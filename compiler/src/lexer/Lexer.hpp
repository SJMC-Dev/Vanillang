#pragma once

#include "token/Token.hpp"
#include <stack>
#include <string_view>

namespace vnlc {
    enum class LexerMode : std::uint8_t {
        DEFAULT,
        SINGLE_QUOTE_STRING,
        DOUBLE_QUOTE_STRING,
        SINGLE_QUOTE_FORMAT_STRING,
        DOUBLE_QUOTE_FORMAT_STRING,
        SINGLE_QUOTE_RAW_STRING,
        DOUBLE_QUOTE_RAW_STRING,
        NUMBER_STARTS_WITH_DOT,
    };

    class Lexer {
    private:
        std::string_view code;

        LexerMode mode = LexerMode::DEFAULT;

        int parenthesisCounter = -1;
        std::stack<int> parenthesisCounterStack;
        std::stack<LexerMode> formatStringModeStack;

        std::size_t lastOffset = 0;
        std::size_t lastLine = 1;
        std::size_t lastColumn = 1;

        std::size_t currentOffset = 0;
        std::size_t currentLine = 1;
        std::size_t currentColumn = 1;

        bool exhausted = false;

        void advance();

        [[nodiscard]] std::string_view value() const noexcept;

        [[nodiscard]] bool blank() const noexcept;
        [[nodiscard]] bool number() const noexcept;
        [[nodiscard]] bool special() const noexcept;
        [[nodiscard]] bool newline() const noexcept;
        [[nodiscard]] bool eof() const noexcept;
        [[nodiscard]] bool separator() const noexcept;

        [[nodiscard]] char peek() const noexcept;
        [[nodiscard]] char peek(std::size_t index) const noexcept;

        [[nodiscard]] Token scanTokenStartsWithNumber();
        [[nodiscard]] Token scanTokenStartsWithSpecial();
        [[nodiscard]] Token scanTokenStartsWithNewline();
        [[nodiscard]] Token scanTokenStartsWithEof();
        [[nodiscard]] Token scanTokenStartsWithIdentifier();
        [[nodiscard]] Token scanStringLiteral();
        [[nodiscard]] Token scanFormatStringLiteral();
        [[nodiscard]] Token scanRawStringLiteral();

        [[nodiscard]] bool hasNext() const noexcept;
        [[nodiscard]] Token next();

    public:
        explicit Lexer(std::string_view code);

        [[nodiscard]] Tokens scan();
    };
} // namespace vnlc
