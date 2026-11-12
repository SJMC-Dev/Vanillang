#pragma once

#include "error/Error.hpp"
#include <fmt/core.h>

namespace vnlc {
    class OutOfRangeError : public Error {
    public:
        OutOfRangeError(std::string_view message) : Error(fmt::format("Out of range: {}", message)) {}
    };
} // namespace vnlc
