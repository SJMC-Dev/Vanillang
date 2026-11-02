#ifndef VNLC_LEXER_HPP
#define VNLC_LEXER_HPP

#include "lexer/LexerMode.hpp"
#include "token/Token.hpp"
#include "token/TokenKindUtil.hpp"
#include <istream>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace vnlc {
    class Lexer {
    private:
        LexerMode mode = LexerMode::DEFAULT;
        std::stack<int, std::vector<int>> parenthesisCounterStack;
        int parenthesisCounter = -1;

        [[nodiscard]] static const std::unordered_map<std::string_view, TokenKind>& getKeywords();

        std::istream& source;
        std::string currentLine;

        std::size_t offset = 0;
        std::size_t line = 0;
        std::size_t column = 1;

        bool exhausted = false;

        void advance();
        bool readline();
        void collect(std::string& tokenValue);

        [[nodiscard]] bool blank() const;
        [[nodiscard]] bool number() const;
        [[nodiscard]] bool special() const;
        [[nodiscard]] bool newline() const;
        [[nodiscard]] bool eof() const;
        [[nodiscard]] bool separator() const;

        [[nodiscard]] int peek() const;
        [[nodiscard]] int peek(std::size_t offset) const;

        [[nodiscard]] Token scanTokenStartsWithBlank(std::string& tokenValue, std::size_t currentLine, std::size_t currentColumn, std::size_t currentOffset);
        [[nodiscard]] Token scanTokenStartsWithNumber(std::string& tokenValue, std::size_t currentLine, std::size_t currentColumn, std::size_t currentOffset);
        [[nodiscard]] Token scanTokenStartsWithSpecial(std::string& tokenValue, std::size_t currentLine, std::size_t currentColumn, std::size_t currentOffset);
        [[nodiscard]] Token scanTokenStartsWithNewline(std::string& tokenValue, std::size_t currentLine, std::size_t currentColumn, std::size_t currentOffset);
        [[nodiscard]] Token scanTokenStartsWithEof(std::string& tokenValue, std::size_t currentLine, std::size_t currentColumn, std::size_t currentOffset);
        [[nodiscard]] Token scanTokenStartsWithIdentifier(std::string& tokenValue, std::size_t currentLine, std::size_t currentColumn, std::size_t currentOffset);
        [[nodiscard]] Token scanStringLiteral(std::string& tokenValue, std::size_t currentLine, std::size_t currentColumn, std::size_t currentOffset);
        [[nodiscard]] Token scanFormatStringLiteral(std::string& tokenValue, std::size_t currentLine, std::size_t currentColumn, std::size_t currentOffset);
        [[nodiscard]] Token scanRawStringLiteral(std::string& tokenValue, std::size_t currentLine, std::size_t currentColumn, std::size_t currentOffset);

    public:
        explicit Lexer(std::istream& input);

        [[nodiscard]] bool hasNext() const;
        [[nodiscard]] Token next();

        friend bool TokenKindUtil::isGeneralizedIdentifier(TokenKind type);
    };
} // namespace vnlc

#endif // VNLC_LEXER_HPP
