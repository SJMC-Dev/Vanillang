#include "Utils.hpp"
#include <algorithm>
#include <cctype>

namespace vnlc {
    bool Utils::hasExtensionIgnoringCase(const std::filesystem::path& path, std::string_view extension) {
        const auto toLower = [](unsigned char c) { return static_cast<char>(std::tolower(c)); };

        return std::ranges::equal(path.extension().string(), extension, {}, toLower, toLower);
    }
} // namespace vnlc
