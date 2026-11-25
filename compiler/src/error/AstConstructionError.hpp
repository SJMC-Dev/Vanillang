#pragma once

#include "error/Error.hpp"
#include <fmt/core.h>

namespace vnlc {
    class AstConstructionError : public Error {
    public:
        AstConstructionError(std::string_view message) : Error(fmt::format("Invalid AST construction: {}", message)) {}
    };
} // namespace vnlc
