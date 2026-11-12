#pragma once

#include "ast/declaration/ImportDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct ImportDeclarationParsingResult {
        std::unique_ptr<ImportDeclarationNode> declaration;
    };
} // namespace vnlc
