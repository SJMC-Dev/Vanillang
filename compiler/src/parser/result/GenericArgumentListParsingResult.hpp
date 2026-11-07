#pragma once

#include "ast/typeannotation/TypeAnnotationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct GenericArgumentListParsingResult {
        std::vector<std::unique_ptr<TypeAnnotationNode>> arguments;
    };
} // namespace vnlc
