#pragma once

#include "ast/declaration/DeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct TopIdentifierDeclarationParsingResult {
        std::unique_ptr<DeclarationNode> declaration;
    };
} // namespace vnlc
