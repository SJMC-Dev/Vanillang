#ifndef VNLC_DIAGNOSTIC_SEVERITY_HPP
#define VNLC_DIAGNOSTIC_SEVERITY_HPP

#include <cstdint>

namespace vnlc {
    enum class DiagnosticSeverity : std::uint8_t {
        ERROR,
        WARNING,
        NOTE,
    };
}

#endif // VNLC_DIAGNOSTIC_SEVERITY_HPP