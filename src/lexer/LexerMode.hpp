#ifndef VNLC_LEXER_MODE_HPP
#define VNLC_LEXER_MODE_HPP

#include <cstdint>

namespace vnlc {
    enum class LexerMode : std::uint8_t {
        DEFAULT,
        STRING,
        FORMAT_STRING,
        RAW_STRING,
        INTERPOLATION_BEGIN,
        NUMBER_STARTS_WITH_DOT,
    };
}

#endif // VNLC_LEXER_MODE_HPP