#include "ImportedIdentifier.hpp"

namespace vnlc {
    ImportedIdentifier::ImportedIdentifier(std::string_view name, std::unordered_map<std::string, std::optional<std::string>>&& metadata) : ImportedItem(name), metadata(std::move(metadata)) {}

    ImportedIdentifier::ImportedIdentifier(std::string_view name) : ImportedItem(name), metadata({}) {}

    const std::unordered_map<std::string, std::optional<std::string>>& ImportedIdentifier::getMetadata() const {
        return metadata;
    }

    std::string_view ImportedIdentifier::getAccessModifier() const {
        return "public";
    }
} // namespace vnlc
