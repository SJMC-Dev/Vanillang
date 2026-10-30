#ifndef VNLC_IMPORT_DECLARATION_ITEM_HPP
#define VNLC_IMPORT_DECLARATION_ITEM_HPP

#include "ast/identifier/IdentifierNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct ImportDeclarationItem {
        std::vector<std::unique_ptr<IdentifierNode>> namePrefix;
        std::vector<std::unique_ptr<ImportDeclarationItem>> nameSuffixes;
        std::unique_ptr<IdentifierNode> alias; // nullptr if no alias
        bool self = false;
        bool wildcard = false;
    };
} // namespace vnlc

#endif // VNLC_IMPORT_DECLARATION_ITEM_HPP