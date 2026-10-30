#ifndef VNLC_FOR_STATEMENT_PARSING_CONTEXT_HPP
#define VNLC_FOR_STATEMENT_PARSING_CONTEXT_HPP

#include "ast/identifier/IdentifierNode.hpp"
#include <memory>

namespace vnlc {
    struct ForStatementParsingContext {
        std::unique_ptr<IdentifierNode> label; // nullptr if no label
    };
} // namespace vnlc

#endif // VNLC_FOR_STATEMENT_PARSING_CONTEXT_HPP