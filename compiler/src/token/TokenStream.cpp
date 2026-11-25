#include "TokenStream.hpp"
#include "error/OutOfRangeError.hpp"
#include <utility>

namespace vnlc {
    TokenStream::TokenStream(Lexer&& lexer, std::size_t maxBufferSize) : lexer(std::move(lexer)), tokenBuffer(), currentTokenIndex(0), bufferSize(maxBufferSize), newlineOrEOFReached(false) {
        fillBuffer();
    }

    bool TokenStream::hasNextToken() const {
        return lexer.hasNext() || currentTokenIndex < bufferSize;
    }

    const Token& TokenStream::peek(std::size_t offset) const {
        if (currentTokenIndex + offset < bufferSize) {
            return tokenBuffer[currentTokenIndex + offset];
        } else {
            throw OutOfRangeError("Peek offset exceeds buffer size");
        }
    }

    const Token& TokenStream::peek() const {
        return peek(0);
    }

    bool TokenStream::endsWithNewlineOrEOF() const {
        return newlineOrEOFReached;
    }

    void TokenStream::fillBuffer() {
        bool blank = false;
        tokenBuffer.clear();

        for (std::size_t index = 0; index < bufferSize && lexer.hasNext(); index = blank ? index : index + 1) {
            Token token = lexer.next();
            if (token.getKind() == TokenKind::BLANK || token.getKind() == TokenKind::SINGLE_LINE_COMMENT || token.getKind() == TokenKind::MULTI_LINE_COMMENT) {
                blank = true;
            } else {
                blank = false;
                tokenBuffer.push_back(std::move(token));
            }
        }

        bufferSize = tokenBuffer.size();
    }

    void TokenStream::advanceRaw() {
        if (peek().getKind() == TokenKind::END_OF_FILE) {
            currentTokenIndex = bufferSize;
            return;
        }

        if (currentTokenIndex < bufferSize) {
            currentTokenIndex++;
            if (currentTokenIndex == bufferSize && lexer.hasNext()) {
                fillBuffer();
                currentTokenIndex = 0;
            }
        }
    }

    void TokenStream::advance() {
        advanceRaw();
        skipNewlines();
    }

    void TokenStream::skipNewlines() {
        bool skipped = false;

        while (hasNextToken() && peek().getKind() == TokenKind::NEWLINE) {
            advanceRaw();
            skipped = true;
        }

        newlineOrEOFReached = skipped || peek().getKind() == TokenKind::END_OF_FILE;
    }

    void TokenStream::replaceCurrent(Token token) {
        if (currentTokenIndex < bufferSize) {
            tokenBuffer[currentTokenIndex] = std::move(token);
        } else {
            throw OutOfRangeError("Current token index exceeds buffer size");
        }
    }

    bool TokenStream::check(TokenKind expectedKind) const {
        return hasNextToken() && peek().getKind() == expectedKind;
    }

    bool TokenStream::match(TokenKind expectedKind) {
        if (check(expectedKind)) {
            advance();
            return true;
        }
        return false;
    }
} // namespace vnlc
