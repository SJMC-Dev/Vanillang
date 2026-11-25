#pragma once

#include "ast/declaration/ValueDeclarationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct EnumAssociatedValueListParsingResult {
        std::vector<std::unique_ptr<ValueDeclarationNode>> associatedValues;
    };
} // namespace vnlc
