#pragma once

#include "ast/declaration/DeclarationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct InterfaceBodyParsingResult {
        std::vector<std::unique_ptr<DeclarationNode>> declarations;
    };
} // namespace vnlc
