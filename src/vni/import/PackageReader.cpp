#include "PackageReader.hpp"
#include "error/PackageReaderError.hpp"
#include "utils/Utils.hpp"
#include "vni/import/ModuleInterfaceFileReader.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace vnlc {
    PackageReader::PackageReader(std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& packages) : packages(packages) {}

    void PackageReader::readPackageFromPath(std::string_view path, const Config& config) {
        readFromAnotherRootPackage(path, config.dependencyPackageRootPaths);
    }

    void readFromCurrentRootPackage(std::string_view path) {
        // TODO: implement reading packages and modules in the same root package
    }

    void PackageReader::readFromAnotherRootPackage(std::string_view path, const std::unordered_map<std::string, std::filesystem::path>& rootPaths) {
        std::vector<std::string> names;
        for (std::size_t start = 0;;) {
            std::size_t end = path.find('.', start);
            std::string_view name = path.substr(start, end == std::string_view::npos ? end : end - start);
            if (name.empty() || name.find_first_of("/\\:") != std::string_view::npos || name.find('\0') != std::string_view::npos) {
                throw PackageReaderError(fmt::format("Invalid imported path: {}", path));
            }
            names.emplace_back(name);
            if (end == std::string_view::npos) break;
            start = end + 1;
        }

        auto rootPath = rootPaths.find(names.front());
        if (rootPath == rootPaths.end()) {
            throw PackageReaderError(fmt::format("Could not find package with name: {}", names.front()));
        }
        std::filesystem::path currentPath = rootPath->second;
        if (!std::filesystem::is_directory(currentPath)) {
            throw PackageReaderError(fmt::format("Package path {} is not a directory", currentPath.string()));
        }
        if (!packages.contains(names.front())) {
            packages.emplace(
                names.front(),
                std::make_unique<ImportedPackage>(
                    names.front(),
                    std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>(),
                    std::unordered_map<std::string, std::unique_ptr<ImportedModule>>()
                )
            );
        }
        ImportedPackage* currentPackage = packages.at(names.front()).get();

        const auto isModuleInterfaceFile = [](const std::filesystem::directory_entry& entry) { return entry.is_regular_file() && Utils::hasExtensionIgnoringCase(entry.path(), ".vni"); };

        for (std::size_t index = 1; index < names.size(); ++index) {
            const std::string& name = names[index];
            std::optional<std::filesystem::path> packagePath;
            std::optional<std::filesystem::path> modulePath;
            for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
                if (entry.is_directory() && entry.path().filename().string() == name) {
                    packagePath = entry.path();
                } else if (isModuleInterfaceFile(entry) && entry.path().stem().string() == name) {
                    if (modulePath.has_value()) {
                        throw PackageReaderError(fmt::format("Found multiple module interface files for module: {}", name));
                    }
                    modulePath = entry.path();
                }
            }

            if (!packagePath.has_value()) {
                if (currentPackage->getModules().contains(name)) return;

                if (!modulePath.has_value()) {
                    throw PackageReaderError(fmt::format("Could not find package or module with name: {}", name));
                }
                ModuleInterfaceFileReader moduleReader(modulePath.value());
                currentPackage->addModule(moduleReader.read());
                return;
            }

            if (!currentPackage->getSubPackages().contains(name)) {
                currentPackage->addSubPackage(
                    std::make_unique<ImportedPackage>(
                        name,
                        std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>(),
                        std::unordered_map<std::string, std::unique_ptr<ImportedModule>>()
                    )
                );
            }
            currentPackage = currentPackage->getSubPackages().at(name).get();
            currentPath = std::move(packagePath.value());
        }

        std::unordered_set<std::filesystem::path> activePackagePaths;
        readPackageContents(currentPath, *currentPackage, activePackagePaths);
    }

    void PackageReader::readPackageContents(const std::filesystem::path& packagePath, ImportedPackage& package, std::unordered_set<std::filesystem::path>& activePackagePaths) {
        std::filesystem::path canonicalPath = std::filesystem::canonical(packagePath);
        if (!activePackagePaths.insert(canonicalPath).second) {
            throw PackageReaderError(fmt::format("Package directory cycle detected at {}", packagePath.string()));
        }
        const auto isModuleInterfaceFile = [](const std::filesystem::directory_entry& entry) { return entry.is_regular_file() && Utils::hasExtensionIgnoringCase(entry.path(), ".vni"); };

        std::unordered_set<std::string> moduleNames;
        for (const auto& entry : std::filesystem::directory_iterator(packagePath)) {
            if (entry.is_directory()) {
                std::string name = entry.path().filename().string();
                if (!package.getSubPackages().contains(name)) {
                    package.addSubPackage(
                        std::make_unique<ImportedPackage>(
                            name,
                            std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>(),
                            std::unordered_map<std::string, std::unique_ptr<ImportedModule>>()
                        )
                    );
                }
                readPackageContents(entry.path(), *package.getSubPackages().at(name), activePackagePaths);
            } else if (isModuleInterfaceFile(entry)) {
                std::string name = entry.path().stem().string();
                if (!moduleNames.insert(name).second) {
                    throw PackageReaderError(fmt::format("Found multiple module interface files for module: {}", name));
                }
                if (!package.getModules().contains(name)) {
                    ModuleInterfaceFileReader moduleReader(entry.path());
                    package.addModule(moduleReader.read());
                }
            }
        }
        activePackagePaths.erase(canonicalPath);
    }
} // namespace vnlc
