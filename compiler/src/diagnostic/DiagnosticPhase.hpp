#ifndef VNLC_DIAGNOSTIC_PHASE_HPP
#define VNLC_DIAGNOSTIC_PHASE_HPP

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

#endif // VNLC_DIAGNOSTIC_PHASE_HPP
