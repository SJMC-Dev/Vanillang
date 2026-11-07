#pragma once

#include "ast/declaration/DeclarationItem.hpp"
#include <vector>

namespace vnlc {
    struct MetadataParsingResult {
        std::vector<DeclarationItem::MetadataTerm> metadata;
    };
} // namespace vnlc
