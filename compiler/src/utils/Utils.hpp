#pragma once

#include <filesystem>
#include <string_view>

namespace vnlc {
    namespace Utils {
        [[nodiscard]] bool hasExtensionIgnoringCase(const std::filesystem::path& path, std::string_view extension);
    };
} // namespace vnlc
