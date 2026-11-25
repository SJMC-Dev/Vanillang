#pragma once

#include "ast/declaration/ValueDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct EnumAssociatedValueParsingResult {
        std::unique_ptr<ValueDeclarationNode> declaration;
    };
} // namespace vnlc
