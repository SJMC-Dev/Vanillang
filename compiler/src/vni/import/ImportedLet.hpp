#ifndef VNLC_IMPORTED_LET_HPP
#define VNLC_IMPORTED_LET_HPP

#include "vni/import/ImportedIdentifier.hpp"

namespace vnlc {
    class ImportedLet : public ImportedIdentifier {
    private:
        std::string type;
        std::string accessModifier;

    public:
        ImportedLet(std::string_view name, std::string_view type, std::string_view accessModifier, std::unordered_map<std::string, std::optional<std::string>>&& metadata);
        ImportedLet(std::string_view name, std::string_view type, std::string_view accessModifier);

        [[nodiscard]] std::string_view getType() const;
        [[nodiscard]] std::string_view getAccessModifier() const override;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_LET_HPP
