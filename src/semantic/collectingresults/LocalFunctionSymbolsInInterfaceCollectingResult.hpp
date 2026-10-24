#ifndef VNLC_LOCAL_FUNCTION_SYMBOLS_IN_INTERFACE_COLLECTING_RESULT_HPP
#define VNLC_LOCAL_FUNCTION_SYMBOLS_IN_INTERFACE_COLLECTING_RESULT_HPP

#include "diagnostic/Diagnostic.hpp"
#include <vector>

namespace vnlc {
    struct LocalFunctionSymbolsInInterfaceCollectingResult {
        std::vector<Diagnostic> errors;
        std::vector<Diagnostic> warnings;
        std::vector<Diagnostic> notes;
    };
} // namespace vnlc

#endif // VNLC_LOCAL_FUNCTION_SYMBOLS_IN_INTERFACE_COLLECTING_RESULT_HPP
