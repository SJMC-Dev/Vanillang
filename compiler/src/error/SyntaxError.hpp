#pragma once

#include "error/Error.hpp"
#include <fmt/core.h>

namespace vnlc {
    class SyntaxError : public Error {
    public:
        SyntaxError(std::string_view message, std::size_t line, std::size_t column) : Error(fmt::format("Syntax error at line {}, column {}: {}", line, column, message)) {}
    };
} // namespace vnlc
