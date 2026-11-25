#pragma once

#include "ast/declaration/EnumMemberDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct EnumMemberDeclarationParsingResult {
        std::unique_ptr<EnumMemberDeclarationNode> declaration;
    };
} // namespace vnlc
