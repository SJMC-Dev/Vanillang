#ifndef VNLC_COLLECTION_RESULT_HPP
#define VNLC_COLLECTION_RESULT_HPP

#include "diagnostic/Diagnostic.hpp"
#include "outline/ModuleOutline.hpp"
#include "vni/import/ImportedRootPackageMap.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class CollectionResult {
    private:
        std::unique_ptr<ModuleOutline> moduleOutline;
        ImportedRootPackageMap imports;
        std::vector<Diagnostic> errors;

    public:
        CollectionResult(std::unique_ptr<ModuleOutline>&& moduleOutline, ImportedRootPackageMap&& imports, std::vector<Diagnostic>&& errors) noexcept;

        CollectionResult(const CollectionResult&) = delete;
        CollectionResult& operator=(const CollectionResult&) = delete;
        CollectionResult(CollectionResult&&) noexcept = default;
        CollectionResult& operator=(CollectionResult&&) noexcept = default;

        [[nodiscard]] const ModuleOutline& getModuleOutline() const noexcept;
        [[nodiscard]] const ImportedRootPackageMap& getImports() const noexcept;
        [[nodiscard]] const std::vector<Diagnostic>& getErrors() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_COLLECTION_RESULT_HPP
