#ifndef VNLC_LOCAL_REGULAR_SYMBOLS_IN_ENUM_COLLECTING_RESULT_HPP
#define VNLC_LOCAL_REGULAR_SYMBOLS_IN_ENUM_COLLECTING_RESULT_HPP

#include "diagnostic/Diagnostic.hpp"
#include <vector>

namespace vnlc {
    struct LocalRegularSymbolsInEnumCollectingResult {
        std::vector<Diagnostic> errors;
        std::vector<Diagnostic> warnings;
        std::vector<Diagnostic> notes;
    };
} // namespace vnlc

#endif // VNLC_LOCAL_REGULAR_SYMBOLS_IN_ENUM_COLLECTING_RESULT_HPP
