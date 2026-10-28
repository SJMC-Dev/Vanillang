#ifndef VNLC_EXPRESSION_ANALYSIS_RESULT_HPP
#define VNLC_EXPRESSION_ANALYSIS_RESULT_HPP

#include "type/Type.hpp"

namespace vnlc {
    struct ExpressionAnalysisResult {
        enum class ValueKind {
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