#pragma once

#include "ast/declaration/ValueDeclarationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct ParameterListParsingResult {
        std::vector<std::unique_ptr<ValueDeclarationNode>> parameters;
    };
} // namespace vnlc
