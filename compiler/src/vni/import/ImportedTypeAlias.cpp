#include "ImportedTypeAlias.hpp"

namespace vnlc {
    ImportedTypeAlias::ImportedTypeAlias(
        std::string_view name,
        std::vector<std::string>&& genericParameters,
        std::string_view originalType,
        std::string_view accessModifier,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    )
        : ImportedIdentifier(name, std::move(metadata)),
          genericParameters(std::move(genericParameters)),
          originalType(originalType),
          accessModifier(accessModifier) {}

    ImportedTypeAlias::ImportedTypeAlias(std::string_view name, std::vector<std::string>&& genericParameters, std::string_view originalType, std::string_view accessModifier)
        : ImportedIdentifier(name),
          genericParameters(std::move(genericParameters)),
          originalType(originalType),
          accessModifier(accessModifier) {}

    const std::vector<std::string>& ImportedTypeAlias::getGenericParameters() const {
        return genericParameters;
    }

    std::string_view ImportedTypeAlias::getOriginalType() const {
        return originalType;
    }

    std::string_view ImportedTypeAlias::getAccessModifier() const {
        return accessModifier;
    }
} // namespace vnlc
