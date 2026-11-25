#pragma once

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
