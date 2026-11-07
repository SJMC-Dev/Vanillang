#ifndef VNLC_CLASS_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_CLASS_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/TypeDeclarationKind.hpp"
#include <vector>

namespace vnlc {
    struct ClassDeclarationParsingContext {
        TypeDeclarationKind::AccessModifier accessModifier;
        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc

#endif // VNLC_CLASS_DECLARATION_PARSING_CONTEXT_HPP