#pragma once

#include "error/Error.hpp"
#include <fmt/core.h>

namespace vnlc {
    class PackageNameConflictError : public Error {
    public:
        PackageNameConflictError(std::string_view packageName) : Error(fmt::format("Found two or more packages with the name: {}", packageName)) {}
    };
} // namespace vnlc
