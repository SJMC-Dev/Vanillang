#pragma once

#include "ast/declaration/OperatorDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct OperatorDeclarationParsingResult {
        std::unique_ptr<OperatorDeclarationNode> declaration;
    };
} // namespace vnlc
