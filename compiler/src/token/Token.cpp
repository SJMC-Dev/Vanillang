#include "Token.hpp"

#define matchTokenKind(kind) \
    case kind:               \
        return "vnlc::" #kind ""

namespace vnlc {
    const std::unordered_map<std::string_view, TokenKind> TokenKindUtil::keywords{
        { "let", TokenKind::LET },
        { "const", TokenKind::CONST_KEYWORD },
        { "byte", TokenKind::BYTE_TYPE },
        { "ubyte", TokenKind::UBYTE_TYPE },
        { "short", TokenKind::SHORT_TYPE },
        { "ushort", TokenKind::USHORT_TYPE },
        { "int", TokenKind::INT_TYPE },
        { "uint", TokenKind::UINT_TYPE },
        { "long", TokenKind::LONG_TYPE },
        { "ulong", TokenKind::ULONG_TYPE },
        { "float", TokenKind::FLOAT_TYPE },
        { "double", TokenKind::DOUBLE_TYPE },
        { "bool", TokenKind::BOOL_TYPE },
        { "func", TokenKind::FUNC },
        { "init", TokenKind::INIT },
        { "op", TokenKind::OP },
        { "return", TokenKind::RETURN },
        { "if", TokenKind::IF },
        { "else", TokenKind::ELSE },
        { "for", TokenKind::FOR },
        { "while", TokenKind::WHILE },
        { "label", TokenKind::LABEL },
        { "break", TokenKind::BREAK },
        { "continue", TokenKind::CONTINUE },
        { "switch", TokenKind::SWITCH },
        { "case", TokenKind::CASE },
        { "default", TokenKind::DEFAULT },
        { "when", TokenKind::WHEN },
        { "context", TokenKind::CONTEXT },
        { "void", TokenKind::VOID_KEYWORD },
        { "native", TokenKind::NATIVE },
        { "callee", TokenKind::CALLEE },
        { "in", TokenKind::IN_KEYWORD },
        { "reload", TokenKind::RELOAD },
        { "try", TokenKind::TRY },
        { "class", TokenKind::CLASS },
        { "interface", TokenKind::INTERFACE },
        { "abstract", TokenKind::ABSTRACT },
        { "type", TokenKind::TYPE },
        { "enum", TokenKind::ENUM },
        { "record", TokenKind::RECORD },
        { "extends", TokenKind::EXTENDS },
        { "implements", TokenKind::IMPLEMENTS },
        { "this", TokenKind::THIS },
        { "super", TokenKind::SUPER },
        { "private", TokenKind::PRIVATE },
        { "protected", TokenKind::PROTECTED },
        { "public", TokenKind::PUBLIC },
        { "static", TokenKind::STATIC },
        { "instanceof", TokenKind::INSTANCEOF },
        { "final", TokenKind::FINAL },
        { "override", TokenKind::OVERRIDE },
        { "none", TokenKind::NONE },
        { "any", TokenKind::ANY },
        { "where", TokenKind::WHERE },
        { "notnone", TokenKind::NOTNONE },
        { "hasop", TokenKind::HASOP },
        { "true", TokenKind::TRUE_LITERAL },
        { "false", TokenKind::FALSE_LITERAL },
        { "import", TokenKind::IMPORT },
        { "export", TokenKind::EXPORT },
        { "as", TokenKind::AS },
        { "self", TokenKind::SELF },
        { "lambda", TokenKind::LAMBDA },
        { "command", TokenKind::COMMAND },
        { "async", TokenKind::ASYNC },
        { "await", TokenKind::AWAIT },
        { "yield", TokenKind::YIELD },
        { "from", TokenKind::FROM },
        { "decorate", TokenKind::DECORATE },
        { "assert", TokenKind::ASSERT },
        { "typeof", TokenKind::TYPEOF },
        { "defer", TokenKind::DEFER },
        { "asset", TokenKind::ASSET },
        { "define", TokenKind::DEFINE },
        { "metadata", TokenKind::METADATA },
    };

    std::string_view TokenKindUtil::toString(TokenKind kind) noexcept {
        switch (kind) {
            matchTokenKind(TokenKind::IDENTIFIER);
            matchTokenKind(TokenKind::NUMBER);
            matchTokenKind(TokenKind::STRING);
            matchTokenKind(TokenKind::LET);
            matchTokenKind(TokenKind::CONST_KEYWORD);
            matchTokenKind(TokenKind::BYTE_TYPE);
            matchTokenKind(TokenKind::UBYTE_TYPE);
            matchTokenKind(TokenKind::SHORT_TYPE);
            matchTokenKind(TokenKind::USHORT_TYPE);
            matchTokenKind(TokenKind::INT_TYPE);
            matchTokenKind(TokenKind::UINT_TYPE);
            matchTokenKind(TokenKind::LONG_TYPE);
            matchTokenKind(TokenKind::ULONG_TYPE);
            matchTokenKind(TokenKind::FLOAT_TYPE);
            matchTokenKind(TokenKind::DOUBLE_TYPE);
            matchTokenKind(TokenKind::BOOL_TYPE);
            matchTokenKind(TokenKind::FUNC);
            matchTokenKind(TokenKind::INIT);
            matchTokenKind(TokenKind::OP);
            matchTokenKind(TokenKind::RETURN);
            matchTokenKind(TokenKind::IF);
            matchTokenKind(TokenKind::ELSE);
            matchTokenKind(TokenKind::FOR);
            matchTokenKind(TokenKind::WHILE);
            matchTokenKind(TokenKind::LABEL);
            matchTokenKind(TokenKind::BREAK);
            matchTokenKind(TokenKind::CONTINUE);
            matchTokenKind(TokenKind::SWITCH);
            matchTokenKind(TokenKind::CASE);
            matchTokenKind(TokenKind::DEFAULT);
            matchTokenKind(TokenKind::WHEN);
            matchTokenKind(TokenKind::CONTEXT);
            matchTokenKind(TokenKind::VOID_KEYWORD);
            matchTokenKind(TokenKind::NATIVE);
            matchTokenKind(TokenKind::CALLEE);
            matchTokenKind(TokenKind::IN_KEYWORD);
            matchTokenKind(TokenKind::RELOAD);
            matchTokenKind(TokenKind::TRY);
            matchTokenKind(TokenKind::CLASS);
            matchTokenKind(TokenKind::INTERFACE);
            matchTokenKind(TokenKind::ABSTRACT);
            matchTokenKind(TokenKind::TYPE);
            matchTokenKind(TokenKind::ENUM);
            matchTokenKind(TokenKind::RECORD);
            matchTokenKind(TokenKind::EXTENDS);
            matchTokenKind(TokenKind::IMPLEMENTS);
            matchTokenKind(TokenKind::THIS);
            matchTokenKind(TokenKind::SUPER);
            matchTokenKind(TokenKind::PRIVATE);
            matchTokenKind(TokenKind::PROTECTED);
            matchTokenKind(TokenKind::PUBLIC);
            matchTokenKind(TokenKind::STATIC);
            matchTokenKind(TokenKind::INSTANCEOF);
            matchTokenKind(TokenKind::FINAL);
            matchTokenKind(TokenKind::OVERRIDE);
            matchTokenKind(TokenKind::NONE);
            matchTokenKind(TokenKind::ANY);
            matchTokenKind(TokenKind::WHERE);
            matchTokenKind(TokenKind::NOTNONE);
            matchTokenKind(TokenKind::HASOP);
            matchTokenKind(TokenKind::TRUE_LITERAL);
            matchTokenKind(TokenKind::FALSE_LITERAL);
            matchTokenKind(TokenKind::IMPORT);
            matchTokenKind(TokenKind::EXPORT);
            matchTokenKind(TokenKind::AS);
            matchTokenKind(TokenKind::MODULE);
            matchTokenKind(TokenKind::LAMBDA);
            matchTokenKind(TokenKind::COMMAND);
            matchTokenKind(TokenKind::ASYNC);
            matchTokenKind(TokenKind::AWAIT);
            matchTokenKind(TokenKind::YIELD);
            matchTokenKind(TokenKind::FROM);
            matchTokenKind(TokenKind::DECORATE);
            matchTokenKind(TokenKind::ASSERT);
            matchTokenKind(TokenKind::TYPEOF);
            matchTokenKind(TokenKind::DEFER);
            matchTokenKind(TokenKind::ASSET);
            matchTokenKind(TokenKind::DEFINE);
            matchTokenKind(TokenKind::METADATA);
            matchTokenKind(TokenKind::PLUS);
            matchTokenKind(TokenKind::MINUS);
            matchTokenKind(TokenKind::ASTERISK);
            matchTokenKind(TokenKind::SLASH);
            matchTokenKind(TokenKind::DOUBLE_SLASH);
            matchTokenKind(TokenKind::PERCENT);
            matchTokenKind(TokenKind::DOUBLE_ASTERISK);
            matchTokenKind(TokenKind::PLUS_EQUAL);
            matchTokenKind(TokenKind::MINUS_EQUAL);
            matchTokenKind(TokenKind::ASTERISK_EQUAL);
            matchTokenKind(TokenKind::SLASH_EQUAL);
            matchTokenKind(TokenKind::DOUBLE_SLASH_EQUAL);
            matchTokenKind(TokenKind::PERCENT_EQUAL);
            matchTokenKind(TokenKind::DOUBLE_ASTERISK_EQUAL);
            matchTokenKind(TokenKind::EQUAL);
            matchTokenKind(TokenKind::DOUBLE_QUESTION_EQUAL);
            matchTokenKind(TokenKind::DOUBLE_EQUAL);
            matchTokenKind(TokenKind::TRIPLE_EQUAL);
            matchTokenKind(TokenKind::EXCLAMATION_EQUAL);
            matchTokenKind(TokenKind::EXCLAMATION_DOUBLE_EQUAL);
            matchTokenKind(TokenKind::RIGHT_ANGLE);
            matchTokenKind(TokenKind::RIGHT_ANGLE_EQUAL);
            matchTokenKind(TokenKind::LEFT_ANGLE);
            matchTokenKind(TokenKind::LEFT_ANGLE_EQUAL);
            matchTokenKind(TokenKind::LEFT_ANGLE_EQUAL_RIGHT_ANGLE);
            matchTokenKind(TokenKind::DOUBLE_AMPERSAND);
            matchTokenKind(TokenKind::DOUBLE_PIPE);
            matchTokenKind(TokenKind::DOUBLE_QUESTION);
            matchTokenKind(TokenKind::EXCLAMATION);
            matchTokenKind(TokenKind::ARROW);
            matchTokenKind(TokenKind::AMPERSAND);
            matchTokenKind(TokenKind::PIPE);
            matchTokenKind(TokenKind::CARET);
            matchTokenKind(TokenKind::TILDE);
            matchTokenKind(TokenKind::DOUBLE_LEFT_ANGLE);
            matchTokenKind(TokenKind::DOUBLE_RIGHT_ANGLE);
            matchTokenKind(TokenKind::TRIPLE_RIGHT_ANGLE);
            matchTokenKind(TokenKind::AMPERSAND_EQUAL);
            matchTokenKind(TokenKind::PIPE_EQUAL);
            matchTokenKind(TokenKind::CARET_EQUAL);
            matchTokenKind(TokenKind::DOUBLE_LEFT_ANGLE_EQUAL);
            matchTokenKind(TokenKind::DOUBLE_RIGHT_ANGLE_EQUAL);
            matchTokenKind(TokenKind::TRIPLE_RIGHT_ANGLE_EQUAL);
            matchTokenKind(TokenKind::DOT);
            matchTokenKind(TokenKind::COMMA);
            matchTokenKind(TokenKind::SEMICOLON);
            matchTokenKind(TokenKind::COLON);
            matchTokenKind(TokenKind::DOUBLE_COLON);
            matchTokenKind(TokenKind::QUESTION);
            matchTokenKind(TokenKind::QUESTION_DOT);
            matchTokenKind(TokenKind::DOUBLE_DOT);
            matchTokenKind(TokenKind::TRIPLE_DOT);
            matchTokenKind(TokenKind::LEFT_PARENTHESIS);
            matchTokenKind(TokenKind::RIGHT_PARENTHESIS);
            matchTokenKind(TokenKind::LEFT_BRACE);
            matchTokenKind(TokenKind::RIGHT_BRACE);
            matchTokenKind(TokenKind::LEFT_BRACKET);
            matchTokenKind(TokenKind::RIGHT_BRACKET);
            matchTokenKind(TokenKind::SELECTOR_PREFIX);
            matchTokenKind(TokenKind::INTERPOLATION_START);
            matchTokenKind(TokenKind::SINGLE_LINE_COMMENT);
            matchTokenKind(TokenKind::MULTI_LINE_COMMENT);
            matchTokenKind(TokenKind::NEWLINE);
            matchTokenKind(TokenKind::LEXICAL_ERROR);
            matchTokenKind(TokenKind::END_OF_FILE);
            default:
                return "Unknown vnlc::TokenKind";
        }
    }

    bool TokenKindUtil::isKeyword(TokenKind kind) noexcept {
        for (auto& [keyword, keywordKind] : keywords) {
            if (kind == keywordKind) {
                return true;
            }
        }

        return false;
    }

    bool TokenKindUtil::isGeneralizedIdentifier(TokenKind kind) noexcept {
        return isKeyword(kind) || kind == TokenKind::IDENTIFIER;
    }

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

#undef matchTokenKind