#ifndef VNLC_EXPORT_CHECKING_RESULT_HPP
#define VNLC_EXPORT_CHECKING_RESULT_HPP

#include "diagnostic/Diagnostic.hpp"
#include <vector>

namespace vnlc {
    struct ExportCheckingResult {
        std::vector<Diagnostic> errors;
        std::vector<Diagnostic> warnings;
        std::vector<Diagnostic> notes;
    };
} // namespace vnlc

#endif // VNLC_EXPORT_CHECKING_RESULT_HPP
