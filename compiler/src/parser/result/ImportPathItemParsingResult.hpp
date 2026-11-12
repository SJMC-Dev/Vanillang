#pragma once

#include "ast/declaration/ImportDeclarationItem.hpp"
#include <memory>

namespace vnlc {
    struct ImportPathItemParsingResult {
        std::unique_ptr<ImportDeclarationItem> paths;
    };
} // namespace vnlc
