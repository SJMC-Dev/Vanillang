#pragma once

#include "error/Error.hpp"
#include <fmt/core.h>

namespace vnlc {
    class InternalError : public Error {
    public:
        InternalError(std::string_view message) : Error(fmt::format("Internal error: {}", message)) {}
    };
} // namespace vnlc
