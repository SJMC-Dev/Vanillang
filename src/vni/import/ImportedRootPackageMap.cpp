#include "ImportedRootPackageMap.hpp"
#include "vni/import/ImportedItem.hpp"

namespace vnlc {
    const ImportedItem* ImportedRootPackageMap::getImportedItemByFullPath(const std::vector<std::string>& fullPath) const {
        if (fullPath.empty()) {
            return nullptr;
        }

        const ImportedPackage* rootPackage = getImportedRootPackageByName(fullPath.front());
        if (!rootPackage) {
            return nullptr;
        }

        const ImportedItem* item = static_cast<const ImportedItem*>(rootPackage);
        for (std::size_t index = 1; index < fullPath.size(); ++index) {
            item = item->getChildByName(fullPath[index]);

            if (item == nullptr) {
                return nullptr;
            }
        }

        return item;
    }

    const ImportedItem* ImportedRootPackageMap::getImportedItemByFullTypeName(std::string_view fullTypeName) const {
        const auto isSpace = [](char character) { return character == ' ' || character == '\t' || character == '\r' || character == '\n'; };

        std::string_view unwrappedTypeName = fullTypeName.substr(0, fullTypeName.find('<'));
        while (!unwrappedTypeName.empty() && isSpace(unwrappedTypeName.front())) {
            unwrappedTypeName.remove_prefix(1);
        }
        while (!unwrappedTypeName.empty() && isSpace(unwrappedTypeName.back())) {
            unwrappedTypeName.remove_suffix(1);
        }

        std::vector<std::string> path;
        std::size_t start = 0;
        while (start <= unwrappedTypeName.size()) {
            const std::size_t separator = unwrappedTypeName.find('.', start);
            if (separator == std::string_view::npos) {
                path.emplace_back(unwrappedTypeName.substr(start));
                break;
            }

            path.emplace_back(unwrappedTypeName.substr(start, separator - start));
            start = separator + 1;
        }

        return getImportedItemByFullPath(path);
    }

    const ImportedPackage* ImportedRootPackageMap::getImportedRootPackageByName(std::string_view name) const {
        auto it = this->find(std::string(name));
        if (it != this->end()) {
            return it->second.get();
        }
        return nullptr;
    }

} // namespace vnlc