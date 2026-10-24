#ifndef VNLC_METADATA_CHECKING_RESULT_HPP
#define VNLC_METADATA_CHECKING_RESULT_HPP

#include "diagnostic/Diagnostic.hpp"
#include "metadata/MetadataInfo.hpp"
#include <vector>

namespace vnlc {
    struct MetadataCheckingResult {
        MetadataInfo metadataInfo;
        std::vector<Diagnostic> errors;
        std::vector<Diagnostic> warnings;
        std::vector<Diagnostic> notes;
    };
} // namespace vnlc

#endif // VNLC_METADATA_CHECKING_RESULT_HPP
