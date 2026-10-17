#ifndef VNLC_UTILS_HPP
#define VNLC_UTILS_HPP

#include <filesystem>
#include <string_view>

namespace vnlc {
    namespace Utils {
        [[nodiscard]] bool hasExtensionIgnoringCase(const std::filesystem::path& path, std::string_view extension);
    }
} // namespace vnlc

#endif // VNLC_UTILS_HPP
