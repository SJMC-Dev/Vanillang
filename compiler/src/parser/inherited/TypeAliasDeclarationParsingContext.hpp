#ifndef VNLC_TYPE_ALIAS_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_TYPE_ALIAS_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/TypeDeclarationKind.hpp"
#include <vector>

namespace vnlc {
    struct TypeAliasDeclarationParsingContext {
        TypeDeclarationKind::AccessModifier accessModifier;
        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc

#endif // VNLC_TYPE_ALIAS_DECLARATION_PARSING_CONTEXT_HPP