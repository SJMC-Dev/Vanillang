#pragma once

#include "ast/typeannotation/TypeAnnotationNode.hpp"
#include <memory>

namespace vnlc {
    struct TypeParsingResult {
        std::unique_ptr<TypeAnnotationNode> type;
    };
} // namespace vnlc
