#pragma once

#include "identifier/IdentifierNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct ImportPathTerm {
        std::vector<std::unique_ptr<IdentifierNode>> namePrefix;
        std::vector<ImportPathTerm> nameSuffixes; // empty if no suffix in braces
        std::unique_ptr<IdentifierNode> alias;    // nullptr if no alias

        bool self = false;
        bool wildcard = false;
    };
} // namespace vnlc