#pragma once

#include "ast/declaration/ImportDeclarationItem.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct ImportPathListParsingResult {
        std::vector<std::unique_ptr<ImportDeclarationItem>> paths;
    };
} // namespace vnlc
