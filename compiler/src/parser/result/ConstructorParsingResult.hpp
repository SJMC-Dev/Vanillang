#pragma once

#include "ast/declaration/ConstructorDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct ConstructorParsingResult {
        std::unique_ptr<ConstructorDeclarationNode> constructor;
    };
} // namespace vnlc
