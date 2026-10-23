#include "ImportedItem.hpp"
#include "vni/import/ImportedModule.hpp"
#include "vni/import/ImportedPackage.hpp"

namespace vnlc {
    ImportedItem::ImportedItem(std::string_view name) : name(name) {}

    std::string_view ImportedItem::getName() const {
        return name;
    }

    const ImportedItem* ImportedItem::getChildByName(std::string_view name) const {
        if (const auto* package = dynamic_cast<const ImportedPackage*>(this)) {
            if (const auto* subPackage = package->getSubPackageByName(name)) {
                return subPackage;
            }
            return package->getModuleByName(name);
        }
        if (const auto* module = dynamic_cast<const ImportedModule*>(this)) {
            return module->getIdentifierByName(name);
        }
        return nullptr;
    }

    const ImportedItem* ImportedItem::getImportedItemByFullPath(const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& imports, const std::vector<std::string>& fullPath) {
        if (fullPath.empty()) {
            return nullptr;
        }

        const auto rootPackage = imports.find(fullPath.front());
        if (rootPackage == imports.end()) {
            return nullptr;
        }

        const ImportedItem* item = rootPackage->second.get();
        for (std::size_t index = 1; index < fullPath.size(); ++index) {
            item = item->getChildByName(fullPath[index]);

            if (item == nullptr) {
                return nullptr;
            }
        }

        return item;
    }

    const ImportedItem* ImportedItem::getImportedItemByFullTypeName(const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& imports, std::string_view fullTypeName) {
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

        return getImportedItemByFullPath(imports, path);
    }
} // namespace vnlc
