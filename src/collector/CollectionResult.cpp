#include "CollectionResult.hpp"

namespace vnlc {
    CollectionResult::CollectionResult(std::unique_ptr<ModuleOutline>&& moduleOutline, ImportedRootPackageMap&& imports, std::vector<Diagnostic>&& errors) noexcept
        : moduleOutline(std::move(moduleOutline)),
          imports(std::move(imports)),
          errors(std::move(errors)) {}

    const ModuleOutline& CollectionResult::getModuleOutline() const noexcept {
        return *moduleOutline;
    }

    const ImportedRootPackageMap& CollectionResult::getImports() const noexcept {
        return imports;
    }

    const std::vector<Diagnostic>& CollectionResult::getErrors() const noexcept {
        return errors;
    }
} // namespace vnlc
