#ifndef VNLC_EXPRESSION_CHECKING_RESULT_HPP
#define VNLC_EXPRESSION_CHECKING_RESULT_HPP

#include "diagnostic/Diagnostic.hpp"
#include <vector>

namespace vnlc {
    struct ExpressionCheckingResult {
        std::vector<Diagnostic> errors;
        std::vector<Diagnostic> warnings;
        std::vector<Diagnostic> notes;
    };
} // namespace vnlc

#endif // VNLC_EXPRESSION_CHECKING_RESULT_HPP
