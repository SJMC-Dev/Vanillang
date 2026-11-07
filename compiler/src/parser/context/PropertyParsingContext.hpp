#pragma once

#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/ValueDeclarationKind.hpp"
#include <vector>

namespace vnlc {
    struct PropertyParsingContext {
        ValueDeclarationKind::Kind kind;
        ValueDeclarationKind::AccessModifier accessModifier;

        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc
