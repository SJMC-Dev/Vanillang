#pragma once

#include <cstdint>

namespace vnlc {
    enum class DiagnosticSeverity : std::uint8_t {
        ERROR,
        WARNING,
        NOTE,
    };
}
