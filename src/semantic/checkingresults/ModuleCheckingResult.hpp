#ifndef VNLC_MODULE_CHECKING_RESULT_HPP
#define VNLC_MODULE_CHECKING_RESULT_HPP

#include "diagnostic/Diagnostic.hpp"
#include <vector>

namespace vnlc {
    struct ModuleCheckingResult {
        std::vector<Diagnostic> errors;
        std::vector<Diagnostic> warnings;
        std::vector<Diagnostic> notes;
    };
} // namespace vnlc

#endif // VNLC_MODULE_CHECKING_RESULT_HPP
