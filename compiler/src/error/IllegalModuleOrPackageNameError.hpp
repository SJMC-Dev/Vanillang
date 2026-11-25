#pragma once

#include "error/Error.hpp"
#include <fmt/core.h>

namespace vnlc {
    class IllegalModuleOrPackageNameError : public Error {
    public:
        IllegalModuleOrPackageNameError(std::string_view message) : Error(fmt::format("Illegal module or package name: {}", message)) {}
    };
} // namespace vnlc
