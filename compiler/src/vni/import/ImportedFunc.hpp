#ifndef VNLC_IMPORTED_FUNC_HPP
#define VNLC_IMPORTED_FUNC_HPP

#include "vni/import/ImportedIdentifier.hpp"
#include "vni/import/ImportedParameter.hpp"
#include <memory>
#include <unordered_map>

namespace vnlc {
    class ImportedFunc : public ImportedIdentifier {
    private:
        std::string originalName;
        std::string returnType;
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>> parameters;
        bool native;
        std::string accessModifier;

    public:
        ImportedFunc(
            std::string_view internalName,
            std::string_view originalName,
            std::string_view returnType,
            std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters,
            bool native,
            std::string_view accessModifier,
            std::unordered_map<std::string, std::optional<std::string>>&& metadata
        );

        ImportedFunc(
            std::string_view internalName,
            std::string_view originalName,
            std::string_view returnType,
            std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters,
            bool native,
            std::string_view accessModifier
        );

        [[nodiscard]] std::string_view getOriginalName() const;
        [[nodiscard]] std::string_view getReturnType() const;
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>& getParameters() const;
        [[nodiscard]] bool isNative() const;
        [[nodiscard]] std::string_view getAccessModifier() const override;

        [[nodiscard]] const ImportedParameter* getParameterByName(std::string_view name) const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_FUNC_HPP
