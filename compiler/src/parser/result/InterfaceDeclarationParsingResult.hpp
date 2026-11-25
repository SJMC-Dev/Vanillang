#pragma once

#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct InterfaceDeclarationParsingResult {
        std::unique_ptr<InterfaceDeclarationNode> declaration;
    };
} // namespace vnlc
