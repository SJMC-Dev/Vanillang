#include "ImportedLet.hpp"

namespace vnlc {
    ImportedLet::ImportedLet(std::string_view name, std::string_view type, std::string_view accessModifier, std::unordered_map<std::string, std::optional<std::string>>&& metadata)
        : ImportedIdentifier(name, std::move(metadata)),
          type(type),
          accessModifier(accessModifier) {}

    ImportedLet::ImportedLet(std::string_view name, std::string_view type, std::string_view accessModifier) : ImportedIdentifier(name), type(type), accessModifier(accessModifier) {}

    std::string_view ImportedLet::getType() const {
        return type;
    }

    std::string_view ImportedLet::getAccessModifier() const {
        return accessModifier;
    }
} // namespace vnlc
