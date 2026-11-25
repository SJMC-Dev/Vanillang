#pragma once

#include "ast/identifier/IdentifierNode.hpp"
#include <memory>
#include <optional>
#include <string>

namespace vnlc {
    namespace DeclarationItem {
        struct MetadataTerm {
            std::unique_ptr<IdentifierNode> key;
            std::optional<std::string> value; // if value is not provided, it is considered as "true"
        };
    }; // namespace DeclarationItem
} // namespace vnlc
