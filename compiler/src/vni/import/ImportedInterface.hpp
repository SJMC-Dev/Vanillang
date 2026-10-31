#ifndef VNLC_IMPORTED_INTERFACE_HPP
#define VNLC_IMPORTED_INTERFACE_HPP

#include "vni/import/ImportedIdentifier.hpp"
#include "vni/import/ImportedMethod.hpp"
#include "vni/import/ImportedOperator.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace vnlc {
    class ImportedInterface : public ImportedIdentifier {
    private:
        std::vector<std::string> genericParameters;
        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>> methods;
        std::unordered_map<std::string, std::unique_ptr<ImportedOperator>> operators;
        std::string accessModifier;

    public:
        ImportedInterface(
            std::string_view name,
            std::vector<std::string>&& genericParameters,
            std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>&& methods,
            std::unordered_map<std::string, std::unique_ptr<ImportedOperator>>&& operators,
            std::string_view accessModifier,
            std::unordered_map<std::string, std::optional<std::string>>&& metadata
        );

        ImportedInterface(
            std::string_view name,
            std::vector<std::string>&& genericParameters,
            std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>&& methods,
            std::unordered_map<std::string, std::unique_ptr<ImportedOperator>>&& operators,
            std::string_view accessModifier
        );

        [[nodiscard]] const std::vector<std::string>& getGenericParameters() const;
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>& getMethods() const;
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedOperator>>& getOperators() const;
        [[nodiscard]] std::string_view getAccessModifier() const override;

        [[nodiscard]] const ImportedMethod* getMethodByName(std::string_view name) const;
        [[nodiscard]] const ImportedOperator* getOperatorByName(std::string_view internalName) const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_INTERFACE_HPP
