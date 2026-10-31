#ifndef VNLC_IMPORTED_TYPE_ALIAS_HPP
#define VNLC_IMPORTED_TYPE_ALIAS_HPP

#include "vni/import/ImportedIdentifier.hpp"
#include <vector>

namespace vnlc {
    class ImportedTypeAlias : public ImportedIdentifier {
    private:
        std::vector<std::string> genericParameters;
        std::string originalType;
        std::string accessModifier;

    public:
        ImportedTypeAlias(
            std::string_view name,
            std::vector<std::string>&& genericParameters,
            std::string_view originalType,
            std::string_view accessModifier,
            std::unordered_map<std::string, std::optional<std::string>>&& metadata
        );
        ImportedTypeAlias(std::string_view name, std::vector<std::string>&& genericParameters, std::string_view originalType, std::string_view accessModifier);

        [[nodiscard]] const std::vector<std::string>& getGenericParameters() const;
        [[nodiscard]] std::string_view getOriginalType() const;
        [[nodiscard]] std::string_view getAccessModifier() const override;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_TYPE_ALIAS_HPP
