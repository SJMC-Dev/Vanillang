#ifndef VNLC_LET_PRIMARY_PARSING_CONTEXT_HPP
#define VNLC_LET_PRIMARY_PARSING_CONTEXT_HPP

#include "ast/declaration/ValueDeclarationKind.hpp"

namespace vnlc {
    struct LetPrimaryParsingContext {
        ValueDeclarationKind::Kind kind;
    };
} // namespace vnlc

#endif // VNLC_LET_PRIMARY_PARSING_CONTEXT_HPP
