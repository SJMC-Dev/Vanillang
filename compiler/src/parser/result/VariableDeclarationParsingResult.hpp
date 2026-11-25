#pragma once

#include "ast/declaration/ValueDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct VariableDeclarationParsingResult {
        std::unique_ptr<ValueDeclarationNode> declaration;
    };
} // namespace vnlc
