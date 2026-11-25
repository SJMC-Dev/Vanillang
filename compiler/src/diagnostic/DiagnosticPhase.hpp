#pragma once

#include <cstdint>

namespace vnlc {
    enum class DiagnosticPhase : std::uint8_t {
        LEXEME,
        COLLECTOR,
        SYNTAX,
        SEMANTIC,
        CODEGEN,
        INTERNAL,
    };
}
