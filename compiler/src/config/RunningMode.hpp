#pragma once

#include <cstdint>

namespace vnlc {
    enum class RunningMode : std::uint8_t {
        COMPILE,
        CHECK,
        LINT,
        FORMAT,
        DUMP_AST,
    };
}
