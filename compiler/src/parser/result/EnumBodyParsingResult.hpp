#pragma once

#include "ast/declaration/EnumMemberDeclarationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct EnumBodyParsingResult {
        std::vector<std::unique_ptr<EnumMemberDeclarationNode>> declarations;
    };
} // namespace vnlc
