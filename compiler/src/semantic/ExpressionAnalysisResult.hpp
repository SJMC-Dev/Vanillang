#pragma once

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
