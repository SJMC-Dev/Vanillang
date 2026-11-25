#pragma once

#include "ast/declaration/FunctionDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct FunctionDeclarationParsingResult {
        std::unique_ptr<FunctionDeclarationNode> declaration;
    };
} // namespace vnlc
