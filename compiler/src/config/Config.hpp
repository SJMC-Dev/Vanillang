#ifndef VNLC_CONFIG_HPP
#define VNLC_CONFIG_HPP

#include "config/RunningMode.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>

namespace vnlc {
    struct Config {
        RunningMode mode;

        std::string vanillangVersion;
        std::string minecraftVersion;

        std::filesystem::path packageRootPath;
        std::filesystem::path inputFilePath;
        std::filesystem::path outputDirectory; // empty if not specified

        // module interface files will not be generated if moduleInterfaceOutputDirectory is empty
        // root package should be a subdirectory of this directory instead of itself
        std::filesystem::path moduleInterfaceOutputDirectory;

        // should include module interface files (.vni) instead of source files (.vnl)
        std::unordered_map<std::string, std::filesystem::path> dependencyPackageRootPaths;

        std::optional<int> optimizationLevel;
    };
} // namespace vnlc

#endif // VNLC_CONFIG_HPP