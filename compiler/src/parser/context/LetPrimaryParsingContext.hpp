#pragma once

#include "ast/declaration/ValueDeclarationKind.hpp"

namespace vnlc {
    struct LetPrimaryParsingContext {
        ValueDeclarationKind::Kind kind;
    };
} // namespace vnlc
