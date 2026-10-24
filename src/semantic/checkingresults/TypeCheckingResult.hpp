#ifndef VNLC_TYPE_CHECKING_RESULT_HPP
#define VNLC_TYPE_CHECKING_RESULT_HPP

#include "diagnostic/Diagnostic.hpp"
#include "type/Type.hpp"
#include <vector>

namespace vnlc {
    struct TypeCheckingResult {
        const Type* type;
        std::vector<Diagnostic> errors;
        std::vector<Diagnostic> warnings;
        std::vector<Diagnostic> notes;
    };
} // namespace vnlc

#endif // VNLC_TYPE_CHECKING_RESULT_HPP
