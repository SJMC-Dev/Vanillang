#include "Utils.hpp"
#include <algorithm>
#include <cctype>
#include <string>

namespace vnlc {
    bool Utils::hasExtensionIgnoringCase(const std::filesystem::path& path, std::string_view extension) {
        const auto toLower = [](unsigned char c) { return static_cast<char>(std::tolower(c)); };

        std::string actualExtension = path.extension().string();
        std::string expectedExtension(extension);
        std::transform(actualExtension.begin(), actualExtension.end(), actualExtension.begin(), toLower);
        std::transform(expectedExtension.begin(), expectedExtension.end(), expectedExtension.begin(), toLower);

        return actualExtension == expectedExtension;
    }
} // namespace vnlc
