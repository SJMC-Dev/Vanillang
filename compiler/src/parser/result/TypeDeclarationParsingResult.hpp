#pragma once

#include "ast/declaration/TypeDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct TypeDeclarationParsingResult {
        std::unique_ptr<TypeDeclarationNode> declaration;
    };
} // namespace vnlc
