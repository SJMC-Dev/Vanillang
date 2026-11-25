#pragma once

#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/FunctionDeclarationKind.hpp"
#include <vector>

namespace vnlc {
    struct FunctionDeclarationParsingContext {
        FunctionDeclarationKind::Implementation implementation;
        FunctionDeclarationKind::Context context;
        FunctionDeclarationKind::AccessModifier accessModifier;
        FunctionDeclarationKind::Binding binding;

        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc
