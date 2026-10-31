#ifndef VNLC_RUNNING_MODE_HPP
#define VNLC_RUNNING_MODE_HPP

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

#endif // VNLC_RUNNING_MODE_HPP