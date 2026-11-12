#pragma once

#include "ast/declaration/EnumDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct EnumDeclarationParsingResult {
        std::unique_ptr<EnumDeclarationNode> declaration;
    };
} // namespace vnlc
