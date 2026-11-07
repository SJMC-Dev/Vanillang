#pragma once

#include "error/Error.hpp"
#include <fmt/core.h>

namespace vnlc {
    class ModuleInterfaceFileReaderError : public Error {
    public:
        ModuleInterfaceFileReaderError(std::string_view message) : Error(fmt::format("Error reading module interface file: {}", message)) {}
    };
} // namespace vnlc
