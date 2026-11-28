#pragma once

#include "identifier/IdentifierNode.hpp"
#include <memory>
#include <optional>

namespace vnlc {
    struct MetadataTerm {
        std::unique_ptr<IdentifierNode> key;
        std::optional<std::string> value;
    };
} // namespace vnlc
