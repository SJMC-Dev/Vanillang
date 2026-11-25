#pragma once

#include "ast/statement/VariableDeclarationStatementNode.hpp"
#include <memory>

namespace vnlc {
    struct VariableDeclarationStatementParsingResult {
        std::unique_ptr<VariableDeclarationStatementNode> statement;
    };
} // namespace vnlc
