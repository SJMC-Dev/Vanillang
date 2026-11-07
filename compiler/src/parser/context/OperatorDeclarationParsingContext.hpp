#pragma once

#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/OperatorDeclarationKind.hpp"
#include <vector>

namespace vnlc {
    struct OperatorDeclarationParsingContext {
        OperatorDeclarationKind::Implementation implementation;
        OperatorDeclarationKind::Context context;
        OperatorDeclarationKind::AccessModifier accessModifier;

        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc
