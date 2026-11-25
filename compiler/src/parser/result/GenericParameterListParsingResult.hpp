#pragma once

#include "ast/identifier/IdentifierNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct GenericParameterListParsingResult {
        std::vector<std::unique_ptr<IdentifierNode>> parameters;
    };
} // namespace vnlc
