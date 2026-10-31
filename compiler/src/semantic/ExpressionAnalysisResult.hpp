#ifndef VNLC_EXPRESSION_ANALYSIS_RESULT_HPP
#define VNLC_EXPRESSION_ANALYSIS_RESULT_HPP

#include "type/Type.hpp"
#include <cstdint>

namespace vnlc {
    struct ExpressionAnalysisResult {
        enum class ValueKind : std::uint8_t {
            PACKAGE,
            MODULE,
            TYPE,
            VALUE,
        };

        ValueKind valueKind;
        const Type* valueType; // nullptr if the value kind is not VALUE
    };
} // namespace vnlc

#endif // VNLC_EXPRESSION_ANALYSIS_RESULT_HPP