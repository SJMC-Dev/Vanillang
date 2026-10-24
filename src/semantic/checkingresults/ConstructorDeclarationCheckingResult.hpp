#ifndef VNLC_CONSTRUCTOR_DECLARATION_CHECKING_RESULT_HPP
#define VNLC_CONSTRUCTOR_DECLARATION_CHECKING_RESULT_HPP

#include "diagnostic/Diagnostic.hpp"
#include <vector>

namespace vnlc {
    struct ConstructorDeclarationCheckingResult {
        std::vector<Diagnostic> errors;
        std::vector<Diagnostic> warnings;
        std::vector<Diagnostic> notes;
    };
} // namespace vnlc

#endif // VNLC_CONSTRUCTOR_DECLARATION_CHECKING_RESULT_HPP
