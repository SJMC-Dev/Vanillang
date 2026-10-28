#ifndef VNLC_IMPORTED_ROOT_PACKAGE_MAP_HPP
#define VNLC_IMPORTED_ROOT_PACKAGE_MAP_HPP

#include "vni/import/ImportedPackage.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace vnlc {
    class ImportedRootPackageMap : public std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> {
    public:
        [[nodiscard]] const ImportedItem* getImportedItemByFullPath(const std::vector<std::string>& fullPath) const;
        [[nodiscard]] const ImportedItem* getImportedItemByFullTypeName(std::string_view fullTypeName) const;
        [[nodiscard]] const ImportedPackage* getImportedRootPackageByName(std::string_view name) const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_ROOT_PACKAGE_MAP_HPP