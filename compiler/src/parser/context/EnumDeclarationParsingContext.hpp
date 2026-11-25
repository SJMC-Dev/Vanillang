#pragma once

#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/TypeDeclarationKind.hpp"
#include <vector>

namespace vnlc {
    struct EnumDeclarationParsingContext {
        TypeDeclarationKind::AccessModifier accessModifier;
        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc
