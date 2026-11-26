#pragma once

#include <string_view>
#include <unordered_map>
#include <vector>

namespace vnlc {
    enum class TokenKind : std::uint8_t {
        IDENTIFIER,

        NUMBER,
        STRING,

        LET,
        CONST_KEYWORD,

        BYTE_TYPE,
        UBYTE_TYPE,
        SHORT_TYPE,
        USHORT_TYPE,
        INT_TYPE,
        UINT_TYPE,
        LONG_TYPE,
        ULONG_TYPE,
        FLOAT_TYPE,
        DOUBLE_TYPE,
        BOOL_TYPE,

        FUNC,
        INIT,
        OP,
        RETURN,
        IF,
        ELSE,
        FOR,
        WHILE,
        LABEL,
        BREAK,
        CONTINUE,
        SWITCH,
        CASE,
        DEFAULT,
        WHEN,
        CONTEXT,
        VOID_KEYWORD,
        NATIVE,
        CALLEE,
        IN_KEYWORD,
        RELOAD,
        TRY,

        CLASS,
        INTERFACE,
        ABSTRACT,
        TYPE,
        ENUM,
        RECORD,
        EXTENDS,
        IMPLEMENTS,
        THIS,
        SUPER,
        PRIVATE,
        PROTECTED,
        PUBLIC,
        STATIC,
        INSTANCEOF,
        FINAL,
        OVERRIDE,
        NONE,
        ANY,

        WHERE,
        NOTNONE,
        HASOP,

        TRUE_LITERAL,
        FALSE_LITERAL,

        IMPORT,
        EXPORT,
        AS,
        MODULE,
        SELF,

        LAMBDA,
        COMMAND,
        ASYNC,
        AWAIT,
        YIELD,
        FROM,
        DECORATE,
        ASSERT,
        TYPEOF,
        DEFER,

        ASSET,
        DEFINE,

        METADATA,

        PLUS,                  // +
        MINUS,                 // -
        ASTERISK,              // *
        SLASH,                 // /
        DOUBLE_SLASH,          // //
        PERCENT,               // %
        DOUBLE_ASTERISK,       // **
        PLUS_EQUAL,            // +=
        MINUS_EQUAL,           // -=
        ASTERISK_EQUAL,        // *=
        SLASH_EQUAL,           // /=
        DOUBLE_SLASH_EQUAL,    // //=
        PERCENT_EQUAL,         // %=
        DOUBLE_ASTERISK_EQUAL, // **=

        EQUAL,                 // =
        DOUBLE_QUESTION_EQUAL, // ??=

        DOUBLE_EQUAL,                 // ==
        TRIPLE_EQUAL,                 // ===
        EXCLAMATION_EQUAL,            // !=
        EXCLAMATION_DOUBLE_EQUAL,     // !==
        RIGHT_ANGLE,                  // >
        RIGHT_ANGLE_EQUAL,            // >=
        LEFT_ANGLE,                   // <
        LEFT_ANGLE_EQUAL,             // <=
        LEFT_ANGLE_EQUAL_RIGHT_ANGLE, // <=>

        DOUBLE_AMPERSAND, // &&
        DOUBLE_PIPE,      // ||
        DOUBLE_QUESTION,  // ??
        EXCLAMATION,      // !

        ARROW, // ->

        AMPERSAND,                // &
        PIPE,                     // |
        CARET,                    // ^
        TILDE,                    // ~
        DOUBLE_LEFT_ANGLE,        // <<
        DOUBLE_RIGHT_ANGLE,       // >>
        TRIPLE_RIGHT_ANGLE,       // >>>
        AMPERSAND_EQUAL,          // &=
        PIPE_EQUAL,               // |=
        CARET_EQUAL,              // ^=
        DOUBLE_LEFT_ANGLE_EQUAL,  // <<=
        DOUBLE_RIGHT_ANGLE_EQUAL, // >>=
        TRIPLE_RIGHT_ANGLE_EQUAL, // >>>=

        DOT,          // .
        COMMA,        // ,
        SEMICOLON,    // ;
        COLON,        // :
        DOUBLE_COLON, // ::
        QUESTION,     // ?
        QUESTION_DOT, // ?.
        DOUBLE_DOT,   // ..
        TRIPLE_DOT,   // ...

        LEFT_PARENTHESIS,  // (
        RIGHT_PARENTHESIS, // )
        LEFT_BRACE,        // {
        RIGHT_BRACE,       // }
        LEFT_BRACKET,      // [
        RIGHT_BRACKET,     // ]

        SELECTOR_PREFIX, // @p, @r, @a, @e, @s, @n

        INTERPOLATION_START, // $(

        SINGLE_LINE_COMMENT, // # comment
        MULTI_LINE_COMMENT,  // #* comment *#

        NEWLINE,

        LEXICAL_ERROR,

        END_OF_FILE,
    };

    namespace TokenKindUtil {
        extern const std::unordered_map<std::string_view, TokenKind> keywords;

        [[nodiscard]] std::string_view toString(TokenKind kind) noexcept;
        [[nodiscard]] bool isKeyword(TokenKind kind) noexcept;
        [[nodiscard]] bool isGeneralizedIdentifier(TokenKind kind) noexcept;
    } // namespace TokenKindUtil

    class Token {
    private:
        TokenKind kind;
        std::string_view value;
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

    using Tokens = std::vector<Token>;
} // namespace vnlc
