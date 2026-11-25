#include "ImportedEnum.hpp"

namespace vnlc {
    ImportedEnum::ImportedEnum(
        std::string_view name,
        std::vector<std::string>&& genericParameters,
        std::unordered_map<std::string, std::unique_ptr<ImportedEnumMember>>&& members,
        std::string_view accessModifier,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    )
        : ImportedIdentifier(name, std::move(metadata)),
          genericParameters(std::move(genericParameters)),
          members(std::move(members)),
          accessModifier(accessModifier) {}

    ImportedEnum::ImportedEnum(
        std::string_view name,
        std::vector<std::string>&& genericParameters,
        std::unordered_map<std::string, std::unique_ptr<ImportedEnumMember>>&& members,
        std::string_view accessModifier
    )
        : ImportedIdentifier(name),
          genericParameters(std::move(genericParameters)),
          members(std::move(members)),
          accessModifier(accessModifier) {}

    const std::vector<std::string>& ImportedEnum::getGenericParameters() const {
        return genericParameters;
    }

    const std::unordered_map<std::string, std::unique_ptr<ImportedEnumMember>>& ImportedEnum::getMembers() const {
        return members;
    }

    std::string_view ImportedEnum::getAccessModifier() const {
        return accessModifier;
    }

    const ImportedEnumMember* ImportedEnum::getMemberByName(std::string_view name) const {
        auto it = members.find(std::string(name));
        if (it != members.end()) {
            return it->second.get();
        }
        return nullptr;
    }
} // namespace vnlc
