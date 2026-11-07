#pragma once

#include "ast/declaration/TypeAliasDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct TypeAliasDeclarationParsingResult {
        std::unique_ptr<TypeAliasDeclarationNode> declaration;
    };
} // namespace vnlc
