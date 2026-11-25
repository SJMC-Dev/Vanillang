#pragma once

#include "ast/declaration/ClassDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct ClassDeclarationParsingResult {
        std::unique_ptr<ClassDeclarationNode> declaration;
    };
} // namespace vnlc
