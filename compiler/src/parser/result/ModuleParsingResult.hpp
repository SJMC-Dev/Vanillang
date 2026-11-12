#pragma once

#include "ast/module/ModuleNode.hpp"
#include <memory>

namespace vnlc {
    struct ModuleParsingResult {
        std::unique_ptr<ModuleNode> moduleNode;
    };
} // namespace vnlc
