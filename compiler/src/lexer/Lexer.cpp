#include "Lexer.hpp"
#include "error/OutOfRangeError.hpp"
#include <string>
#include <string_view>
#include <unordered_map>

namespace vnlc {
    Lexer::Lexer(std::string_view code) : code(code) {}

    char Lexer::peek() const noexcept {
        return peek(0);
    }

    char Lexer::peek(std::size_t index) const noexcept {
        std::size_t target = currentOffset + index;
        if (target < code.size()) {
            return code[target];
        }

        return static_cast<char>(std::char_traits<char>::eof());
    }

    bool Lexer::blank() const noexcept {
        char ch = peek();
        return ch == ' ' || ch == '\t' || ch == '\r';
    }

    bool Lexer::number() const noexcept {
        char ch = peek();
        return ch >= '0' && ch <= '9';
    }

    bool Lexer::special() const noexcept {
        static constexpr std::string_view specialChars = "+-*/=!<>|&^%~?:.,;(){}[]#@$\\`\"'";

        char ch = peek();
        return specialChars.find(ch) != std::string_view::npos;
    }

    bool Lexer::newline() const noexcept {
        return peek() == '\n';
    }

    bool Lexer::eof() const noexcept {
        return peek() == static_cast<char>(std::char_traits<char>::eof());
    }

    bool Lexer::separator() const noexcept {
        return eof() || blank() || special() || newline();
    }

    void Lexer::advance() {
        if (exhausted) {
            throw OutOfRangeError("No more tokens to read.");
        }

        currentOffset++;

        if (eof()) {
            exhausted = true;
            return;
        }

        if (newline()) {
            currentLine++;
            currentLine = 1;
        }
    }

    std::string_view Lexer::value() const noexcept {
        return code.substr(lastOffset, currentOffset - lastOffset);
    }

    bool Lexer::hasNext() const noexcept {
        return !exhausted;
    }

    Token Lexer::scanTokenStartsWithNumber() {
        enum class Base : std::uint8_t {
            DEC,
            BIN,
            OCT,
            HEX,
        };

        static const std::unordered_map<char, Base> baseFlags{
            { 'b', Base::BIN }, { 'B', Base::BIN }, { 'o', Base::OCT }, { 'O', Base::OCT }, { 'x', Base::HEX }, { 'X', Base::HEX },
        };

        bool existE = false;
        bool existDot = mode == LexerMode::NUMBER_STARTS_WITH_DOT;
        bool nonDecimal = false;
        Base base = Base::DEC;

        mode = LexerMode::DEFAULT;

        std::string_view hexDigits = "0123456789abcdefABCDEF";
        std::string_view octalDigits = "01234567";
        std::string_view binaryDigits = "01";
        std::string_view nonDecimalFlags = "xobXOB";
        std::string_view exponentFlags = "eE";
        std::string_view signSuffixes = "sSuU";
        std::string_view integerSuffixes = "bBsSiIlL";
        std::string_view floatSuffixes = "fFdD";

        const auto lexicalError = [&]() -> Token {
            while (!separator()) {
                advance();
            }

            return { TokenKind::LEXICAL_ERROR, value(), lastLine, lastColumn, lastOffset };
        };

        const auto digit = [&]() {
            switch (base) {
                case Base::BIN:
                    return binaryDigits.find(peek()) != std::string_view::npos;
                case Base::OCT:
                    return octalDigits.find(peek()) != std::string_view::npos;
                case Base::HEX:
                    return hexDigits.find(peek()) != std::string_view::npos;
                case Base::DEC:
                    return number();
            }

            return false;
        };

        while (true) {
            if (peek() != '.' && separator()) {
                break;
            }

            if (currentOffset - lastOffset == 0 && peek() == '0' && nonDecimalFlags.find(static_cast<char>(peek(1))) != std::string_view::npos) {
                nonDecimal = true;
                advance();
                base = baseFlags.at(peek());
                advance();

                if (base == Base::BIN && separator()) {
                    break;
                }

                if (!digit()) {
                    return lexicalError();
                }

                continue;
            }

            if (peek() == '.') {
                if (peek(1) == '.') {
                    break;
                }

                advance();

                if (nonDecimal || existE || existDot) {
                    return lexicalError();
                }

                existDot = true;
                continue;
            }

            if (!nonDecimal && exponentFlags.find(peek()) != std::string_view::npos) {
                advance();

                if (existE) {
                    return lexicalError();
                }

                if (peek() == '+' || peek() == '-') {
                    advance();
                }

                if (!number()) {
                    return lexicalError();
                }

                advance();
                existE = true;
                continue;
            }

            if (!digit()) {
                if (existDot || existE) {
                    if (floatSuffixes.find(peek()) == std::string_view::npos) {
                        return lexicalError();
                    }
                } else {
                    if (signSuffixes.find(peek()) != std::string_view::npos && integerSuffixes.find(static_cast<char>(peek(1))) != std::string_view::npos) {
                        advance();
                    }

                    if (integerSuffixes.find(peek()) == std::string_view::npos) {
                        return lexicalError();
                    }
                }

                advance();

                if (!separator()) {
                    return lexicalError();
                }

                break;
            }

            advance();
        }

        return { TokenKind::NUMBER, value(), lastLine, lastColumn, lastOffset };
    }

    Token Lexer::scanTokenStartsWithSpecial() {
        if (peek() == '+') {
            advance();

            if (peek() == '=') {
                advance();
                return { TokenKind::PLUS_EQUAL, value(), lastLine, lastColumn, lastOffset };
            } else if (peek() == '+') {
                advance();
                return { TokenKind::LEXICAL_ERROR, value(), lastLine, lastColumn, lastOffset };
            } else {
                return { TokenKind::PLUS, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '-') {
            advance();

            if (peek() == '=') {
                advance();
                return { TokenKind::MINUS_EQUAL, value(), lastLine, lastColumn, lastOffset };
            } else if (peek() == '-') {
                advance();
                return { TokenKind::LEXICAL_ERROR, value(), lastLine, lastColumn, lastOffset };
            } else if (peek() == '>') {
                advance();
                return { TokenKind::ARROW, value(), lastLine, lastColumn, lastOffset };
            } else {
                return { TokenKind::MINUS, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '*') {
            advance();

            if (peek() == '=') {
                advance();
                return { TokenKind::ASTERISK_EQUAL, value(), lastLine, lastColumn, lastOffset };
            } else if (peek() == '*') {
                advance();

                if (peek() == '=') {
                    advance();
                    return { TokenKind::DOUBLE_ASTERISK_EQUAL, value(), lastLine, lastColumn, lastOffset };
                } else {
                    return { TokenKind::DOUBLE_ASTERISK, value(), lastLine, lastColumn, lastOffset };
                }
            } else {
                return { TokenKind::ASTERISK, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '/') {
            advance();

            if (peek() == '=') {
                advance();
                return { TokenKind::SLASH_EQUAL, value(), lastLine, lastColumn, lastOffset };
            } else if (peek() == '/') {
                advance();

                if (peek() == '=') {
                    advance();
                    return { TokenKind::DOUBLE_SLASH_EQUAL, value(), lastLine, lastColumn, lastOffset };
                } else {
                    return { TokenKind::DOUBLE_SLASH, value(), lastLine, lastColumn, lastOffset };
                }
            } else {
                return { TokenKind::SLASH, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '%') {
            advance();

            if (peek() == '=') {
                advance();
                return { TokenKind::PERCENT_EQUAL, value(), lastLine, lastColumn, lastOffset };
            } else {
                return { TokenKind::PERCENT, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '<') {
            advance();

            if (peek() == '=') {
                advance();

                if (peek() == '>') {
                    advance();
                    return { TokenKind::LEFT_ANGLE_EQUAL_RIGHT_ANGLE, value(), lastLine, lastColumn, lastOffset };
                } else {
                    return { TokenKind::LEFT_ANGLE_EQUAL, value(), lastLine, lastColumn, lastOffset };
                }
            } else if (peek() == '<') {
                advance();

                if (peek() == '=') {
                    advance();
                    return { TokenKind::DOUBLE_LEFT_ANGLE_EQUAL, value(), lastLine, lastColumn, lastOffset };
                } else {
                    return { TokenKind::DOUBLE_LEFT_ANGLE, value(), lastLine, lastColumn, lastOffset };
                }
            } else {
                return { TokenKind::LEFT_ANGLE, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '>') {
            advance();

            if (peek() == '=') {
                advance();
                return { TokenKind::RIGHT_ANGLE_EQUAL, value(), lastLine, lastColumn, lastOffset };
            } else if (peek() == '>') {
                advance();

                if (peek() == '=') {
                    advance();
                    return { TokenKind::DOUBLE_RIGHT_ANGLE_EQUAL, value(), lastLine, lastColumn, lastOffset };
                } else if (peek() == '>') {
                    advance();

                    if (peek() == '=') {
                        advance();
                        return { TokenKind::TRIPLE_RIGHT_ANGLE_EQUAL, value(), lastLine, lastColumn, lastOffset };
                    } else {
                        return { TokenKind::TRIPLE_RIGHT_ANGLE, value(), lastLine, lastColumn, lastOffset };
                    }
                } else {
                    return { TokenKind::DOUBLE_RIGHT_ANGLE, value(), lastLine, lastColumn, lastOffset };
                }
            } else {
                return { TokenKind::RIGHT_ANGLE, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '=') {
            advance();

            if (peek() == '=') {
                advance();

                if (peek() == '=') {
                    advance();
                    return { TokenKind::TRIPLE_EQUAL, value(), lastLine, lastColumn, lastOffset };
                } else {
                    return { TokenKind::DOUBLE_EQUAL, value(), lastLine, lastColumn, lastOffset };
                }
            } else {
                return { TokenKind::EQUAL, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '?') {
            advance();

            if (peek() == '?') {
                advance();

                if (peek() == '=') {
                    advance();
                    return { TokenKind::DOUBLE_QUESTION_EQUAL, value(), lastLine, lastColumn, lastOffset };
                } else {
                    return { TokenKind::DOUBLE_QUESTION, value(), lastLine, lastColumn, lastOffset };
                }
            } else if (peek() == '.') {
                advance();
                return { TokenKind::QUESTION_DOT, value(), lastLine, lastColumn, lastOffset };
            } else {
                return { TokenKind::QUESTION, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '!') {
            advance();

            if (peek() == '=') {
                advance();

                if (peek() == '=') {
                    advance();
                    return { TokenKind::EXCLAMATION_DOUBLE_EQUAL, value(), lastLine, lastColumn, lastOffset };
                } else {
                    return { TokenKind::EXCLAMATION_EQUAL, value(), lastLine, lastColumn, lastOffset };
                }
            } else {
                return { TokenKind::EXCLAMATION, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '&') {
            advance();

            if (peek() == '&') {
                advance();
                return { TokenKind::DOUBLE_AMPERSAND, value(), lastLine, lastColumn, lastOffset };
            } else if (peek() == '=') {
                advance();
                return { TokenKind::AMPERSAND_EQUAL, value(), lastLine, lastColumn, lastOffset };
            } else {
                return { TokenKind::AMPERSAND, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '|') {
            advance();

            if (peek() == '|') {
                advance();
                return { TokenKind::DOUBLE_PIPE, value(), lastLine, lastColumn, lastOffset };
            } else if (peek() == '=') {
                advance();
                return { TokenKind::PIPE_EQUAL, value(), lastLine, lastColumn, lastOffset };
            } else {
                return { TokenKind::PIPE, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '^') {
            advance();

            if (peek() == '=') {
                advance();
                return { TokenKind::CARET_EQUAL, value(), lastLine, lastColumn, lastOffset };
            } else {
                return { TokenKind::CARET, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '~') {
            advance();
            return { TokenKind::TILDE, value(), lastLine, lastColumn, lastOffset };
        } else if (peek() == '.') {
            advance();

            if (peek() == '.') {
                advance();

                if (peek() == '.') {
                    advance();
                    return { TokenKind::TRIPLE_DOT, value(), lastLine, lastColumn, lastOffset };
                } else {
                    return { TokenKind::DOUBLE_DOT, value(), lastLine, lastColumn, lastOffset };
                }
            } else if (number()) {
                mode = LexerMode::NUMBER_STARTS_WITH_DOT;
                return scanTokenStartsWithNumber();
            } else {
                return { TokenKind::DOT, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == ',') {
            advance();
            return { TokenKind::COMMA, value(), lastLine, lastColumn, lastOffset };
        } else if (peek() == ';') {
            advance();
            return { TokenKind::SEMICOLON, value(), lastLine, lastColumn, lastOffset };
        } else if (peek() == ':') {
            advance();

            if (peek() == ':') {
                advance();
                return { TokenKind::DOUBLE_COLON, value(), lastLine, lastColumn, lastOffset };
            } else {
                return { TokenKind::COLON, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '(') {
            advance();

            if (parenthesisCounter >= 0) {
                parenthesisCounter++;
            }

            return { TokenKind::LEFT_PARENTHESIS, value(), lastLine, lastColumn, lastOffset };
        } else if (peek() == ')') {
            advance();

            if (parenthesisCounter > 0) {
                parenthesisCounter--;
                if (parenthesisCounter == 0) {
                    parenthesisCounter = parenthesisCounterStack.top();
                    parenthesisCounterStack.pop();

                    mode = formatStringModeStack.top();
                    formatStringModeStack.pop();
                }
            }

            return { TokenKind::RIGHT_PARENTHESIS, value(), lastLine, lastColumn, lastOffset };
        } else if (peek() == '{') {
            advance();
            return { TokenKind::LEFT_BRACE, value(), lastLine, lastColumn, lastOffset };
        } else if (peek() == '}') {
            advance();
            return { TokenKind::RIGHT_BRACE, value(), lastLine, lastColumn, lastOffset };
        } else if (peek() == '[') {
            advance();
            return { TokenKind::LEFT_BRACKET, value(), lastLine, lastColumn, lastOffset };
        } else if (peek() == ']') {
            advance();
            return { TokenKind::RIGHT_BRACKET, value(), lastLine, lastColumn, lastOffset };
        } else if (peek() == '#') {
            advance();

            if (peek() == '*') {
                advance();

                while (true) {
                    if (eof()) {
                        return { TokenKind::LEXICAL_ERROR, value(), lastLine, lastColumn, lastOffset };
                    }

                    if (peek() == '*' && peek(1) == '#') {
                        advance();
                        advance();
                        break;
                    }

                    advance();
                }

                return { TokenKind::MULTI_LINE_COMMENT, value(), lastLine, lastColumn, lastOffset };
            } else {
                while (!eof() && !newline() && peek() != '\r') {
                    advance();
                }

                return { TokenKind::SINGLE_LINE_COMMENT, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '@') {
            advance();

            std::string_view selectorPrefixes = "praesn";

            if (selectorPrefixes.find(peek()) != std::string_view::npos) {
                advance();

                if (!separator()) {
                    while (!separator()) {
                        advance();
                    }

                    return { TokenKind::LEXICAL_ERROR, value(), lastLine, lastColumn, lastOffset };
                }

                return { TokenKind::SELECTOR_PREFIX, value(), lastLine, lastColumn, lastOffset };
            } else {
                while (!separator()) {
                    advance();
                }

                return { TokenKind::LEXICAL_ERROR, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '$') {
            advance();

            if ((mode == LexerMode::SINGLE_QUOTE_FORMAT_STRING || mode == LexerMode::DOUBLE_QUOTE_FORMAT_STRING) && peek() == '(') {
                advance();

                formatStringModeStack.push(mode);
                parenthesisCounterStack.push(parenthesisCounter);
                parenthesisCounter = 1;
                mode = LexerMode::DEFAULT;

                return { TokenKind::INTERPOLATION_START, value(), lastLine, lastColumn, lastOffset };
            } else {
                while (!separator()) {
                    advance();
                }

                return { TokenKind::LEXICAL_ERROR, value(), lastLine, lastColumn, lastOffset };
            }
        } else if (peek() == '"' || peek() == '\'') {
            advance();
            mode = peek() == '"' ? LexerMode::DOUBLE_QUOTE_STRING : LexerMode::SINGLE_QUOTE_STRING;
            return scanStringLiteral();
        } else {
            advance();

            while (!separator()) {
                advance();
            }

            return { TokenKind::LEXICAL_ERROR, value(), lastLine, lastColumn, lastOffset };
        }
    }

    Token Lexer::scanTokenStartsWithNewline() {
        advance();
        return { TokenKind::NEWLINE, value(), lastLine, lastColumn, lastOffset };
    }

    Token Lexer::scanTokenStartsWithEof() {
        advance();
        return { TokenKind::END_OF_FILE, "", lastLine, lastColumn, lastOffset };
    }

    Token Lexer::scanTokenStartsWithIdentifier() {
        bool error = false;

        if ((peek() == 'f' || peek() == 'F') && (peek(1) == '"' || peek(1) == '\'')) {
            advance();
            advance();

            mode = peek(1) == '"' ? LexerMode::DOUBLE_QUOTE_FORMAT_STRING : LexerMode::SINGLE_QUOTE_FORMAT_STRING;
            return scanFormatStringLiteral();
        }

        if ((peek() == 'r' || peek() == 'R') && (peek(1) == '"' || peek(1) == '\'')) {
            advance();
            advance();

            mode = peek(1) == '"' ? LexerMode::DOUBLE_QUOTE_RAW_STRING : LexerMode::SINGLE_QUOTE_RAW_STRING;
            return scanRawStringLiteral();
        }

        while (!eof() && !blank() && !special() && !newline()) {
            char ch = peek();

            bool isNumber = (ch >= '0' && ch <= '9');
            bool isUpperCase = (ch >= 'A' && ch <= 'Z');
            bool isLowerCase = (ch >= 'a' && ch <= 'z');
            bool isUnderline = (ch == '_');

            if (!(isNumber || isUpperCase || isLowerCase || isUnderline)) {
                advance();
                error = true;
                break;
            }

            advance();
        }

        if (error) {
            while (!separator()) {
                advance();
            }

            return { TokenKind::LEXICAL_ERROR, value(), lastLine, lastColumn, lastOffset };
        } else if (auto keyword = TokenKindUtil::keywords.find(value()); keyword != TokenKindUtil::keywords.end()) {
            return { keyword->second, value(), lastLine, lastColumn, lastOffset };
        } else {
            return { TokenKind::IDENTIFIER, value(), lastLine, lastColumn, lastOffset };
        }
    }

    Token Lexer::scanStringLiteral() {
        bool error = false;
        std::string_view escapeChars = "bfnrst\\\"'";

        while (true) {
            if (eof() || newline()) {
                mode = LexerMode::DEFAULT;
                return { TokenKind::LEXICAL_ERROR, value(), lastLine, lastColumn, lastOffset };
            }

            if (peek() == '\\') {
                advance();

                if (escapeChars.find(peek()) == std::string_view::npos) {
                    error = true;
                }

                advance();
                continue;
            }

            if ((mode == LexerMode::SINGLE_QUOTE_STRING && peek() == '\'') || (mode == LexerMode::DOUBLE_QUOTE_STRING && peek() == '"')) {
                advance();
                break;
            }

            advance();
        }

        mode = LexerMode::DEFAULT;

        if (error) {
            return { TokenKind::LEXICAL_ERROR, value(), lastLine, lastColumn, lastOffset };
        }

        return { TokenKind::STRING, value(), lastLine, lastColumn, lastOffset };
    }

    Token Lexer::scanFormatStringLiteral() {
        bool error = false;
        std::string_view escapeChars = "bfnrst\\\"'$";

        while (true) {
            if (eof() || newline()) {
                mode = LexerMode::DEFAULT;
                return { TokenKind::LEXICAL_ERROR, value(), lastLine, lastColumn, lastOffset };
            }

            if (peek() == '\\') {
                advance();

                if (escapeChars.find(peek()) == std::string_view::npos) {
                    error = true;
                }

                advance();
                continue;
            }

            if ((mode == LexerMode::SINGLE_QUOTE_FORMAT_STRING && peek() == '\'') || (mode == LexerMode::DOUBLE_QUOTE_FORMAT_STRING && peek() == '"')) {
                advance();
                mode = LexerMode::DEFAULT;
                break;
            }

            if (peek() == '$' && peek(1) == '(') {
                break;
            }

            advance();
        }

        if (error) {
            return { TokenKind::LEXICAL_ERROR, value(), lastLine, lastColumn, lastOffset };
        }

        return { TokenKind::STRING, value(), lastLine, lastColumn, lastOffset };
    }

    Token Lexer::scanRawStringLiteral() {
        while (true) {
            if (eof() || newline()) {
                mode = LexerMode::DEFAULT;
                return { TokenKind::LEXICAL_ERROR, value(), lastLine, lastColumn, lastOffset };
            }

            if ((mode == LexerMode::SINGLE_QUOTE_RAW_STRING && peek() == '\'') || (mode == LexerMode::DOUBLE_QUOTE_RAW_STRING && peek() == '"')) {
                advance();
                break;
            }

            advance();
        }

        mode = LexerMode::DEFAULT;
        return { TokenKind::STRING, value(), lastLine, lastColumn, lastOffset };
    }

    Token Lexer::next() {
        if (!hasNext()) {
            throw OutOfRangeError("No more tokens to read.");
        }

        while (blank()) {
            advance();
        }

        lastOffset = currentOffset;
        lastLine = currentLine;
        lastColumn = currentLine;

        if (mode == LexerMode::SINGLE_QUOTE_FORMAT_STRING || mode == LexerMode::DOUBLE_QUOTE_FORMAT_STRING) {
            return scanFormatStringLiteral();
        } else {
            if (eof()) {
                return scanTokenStartsWithEof();
            } else if (newline()) {
                return scanTokenStartsWithNewline();
            } else if (special()) {
                return scanTokenStartsWithSpecial();
            } else if (number()) {
                return scanTokenStartsWithNumber();
            } else {
                return scanTokenStartsWithIdentifier();
            }
        }

        return { TokenKind::LEXICAL_ERROR, value(), lastLine, lastColumn, lastOffset };
    }
} // namespace vnlc
