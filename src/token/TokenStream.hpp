#ifndef VNLC_TOKEN_STREAM_HPP
#define VNLC_TOKEN_STREAM_HPP

#include "lexer/Lexer.hpp"
#include "token/Token.hpp"
#include "token/TokenKind.hpp"
#include <vector>

namespace vnlc {
    class TokenStream {
    private:
        Lexer lexer;

        std::vector<Token> tokenBuffer;
        std::size_t currentTokenIndex;
        std::size_t bufferSize;
        bool newlineOrEOFReached;

        void fillBuffer();
        void advanceRaw();

    public:
        explicit TokenStream(Lexer&& lexer, std::size_t maxBufferSize = 3);

        [[nodiscard]] bool hasNextToken() const;
        [[nodiscard]] const Token& peek() const;
        [[nodiscard]] const Token& peek(std::size_t offset) const;
        [[nodiscard]] bool endsWithNewlineOrEOF() const;

        void advance();
        void skipNewlines();
        void replaceCurrent(Token token);

        [[nodiscard]] bool check(TokenKind expectedKind) const;
        [[nodiscard]] bool match(TokenKind expectedKind);
    };
} // namespace vnlc

#endif // VNLC_TOKEN_STREAM_HPP
