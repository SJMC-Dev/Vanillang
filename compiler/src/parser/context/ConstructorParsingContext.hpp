#pragma once

#include "ast/declaration/ConstructorDeclarationKind.hpp"
#include "ast/declaration/DeclarationItem.hpp"
#include <vector>

namespace vnlc {
    struct ConstructorParsingContext {
        ConstructorDeclarationKind::AccessModifier accessModifier;

        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc
