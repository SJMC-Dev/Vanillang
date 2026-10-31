#ifndef VNLC_VARIABLE_DECLARATION_PARSING_CONTEXT_HPP
#define VNLC_VARIABLE_DECLARATION_PARSING_CONTEXT_HPP

#include "ast/declaration/DeclarationItem.hpp"
#include <cstdint>
#include <vector>

namespace vnlc {
    struct VariableDeclarationParsingContext {
        enum class Position : std::uint8_t {
            TOP_LEVEL,
            STATEMENT,
        };

        Position position;
        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc

#endif // VNLC_VARIABLE_DECLARATION_PARSING_CONTEXT_HPP