#pragma once

#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/ValueDeclarationKind.hpp"
#include <cstdint>
#include <vector>

namespace vnlc {
    struct VariableDeclarationParsingContext {
        enum class Position : std::uint8_t {
            TOP_LEVEL,
            STATEMENT,
        };

        Position position;
        ValueDeclarationKind::AccessModifier accessModifier;
        bool hasMetadata;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
    };
} // namespace vnlc
