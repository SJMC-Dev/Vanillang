#ifndef VNLC_PACKAGE_READER_HPP
#define VNLC_PACKAGE_READER_HPP

#include "config/Config.hpp"
#include "vni/import/ImportedPackage.hpp"
#include "vni/import/ImportedRootPackageMap.hpp"
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace vnlc {
    class PackageReader {
    private:
        ImportedRootPackageMap& packages;

        void readFromCurrentRootPackage(std::string_view path);
        void readFromAnotherRootPackage(std::string_view path, const std::unordered_map<std::string, std::filesystem::path>& rootPaths);

        void readPackageContents(const std::filesystem::path& packagePath, ImportedPackage& package, std::unordered_set<std::filesystem::path>& activePackagePaths);

    public:
        PackageReader(ImportedRootPackageMap& packages);

        void readPackageFromPath(std::string_view path, const Config& config);
    };
} // namespace vnlc

#endif // VNLC_PACKAGE_READER_HPP
