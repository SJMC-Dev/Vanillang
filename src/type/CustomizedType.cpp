#include "CustomizedType.hpp"
#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include "type/CustomizedTypeOrigin.hpp"
#include "type/PrimitiveType.hpp"
#include "vni/import/ImportedClass.hpp"
#include "vni/import/ImportedInterface.hpp"

namespace vnlc {
    CustomizedType::CustomizedType(CustomizedTypeKind customizedKind, std::string_view fullTypeName, std::vector<const Type*>&& genericArguments, const TypeDeclarationNode* localNode)
        : customizedKind(customizedKind),
          origin(CustomizedTypeOrigin::LOCAL),
          fullTypeName(fullTypeName),
          genericArguments(std::move(genericArguments)),
          localNode(localNode),
          importedNode(nullptr) {}

    CustomizedType::CustomizedType(CustomizedTypeKind customizedKind, std::string_view fullTypeName, std::vector<const Type*>&& genericArguments, const ImportedIdentifier* importedNode)
        : customizedKind(customizedKind),
          origin(CustomizedTypeOrigin::IMPORTED),
          fullTypeName(fullTypeName),
          genericArguments(std::move(genericArguments)),
          localNode(nullptr),
          importedNode(importedNode) {}

    CustomizedTypeKind CustomizedType::getCustomizedKind() const noexcept {
        return customizedKind;
    }

    CustomizedTypeOrigin CustomizedType::getOrigin() const noexcept {
        return origin;
    }

    std::string_view CustomizedType::getFullTypeName() const noexcept {
        return fullTypeName;
    }

    bool CustomizedType::canConvertTo(const Type& other) const noexcept {
        if (dynamic_cast<const PrimitiveType*>(&other) != nullptr) {
            return false;
        }

        const auto* customizedType = dynamic_cast<const CustomizedType*>(&other);
        if (customizedType == nullptr) return false;
        if (fullTypeName == customizedType->fullTypeName) return true;

        // TODO: Check base classes and interfaces once the inheritance graph is available.
        return false;
    }

    bool CustomizedType::canUseOperator(std::string_view name) const noexcept {
        if (customizedKind == CustomizedTypeKind::GENERIC_PARAMETER || name == "assignment") return true;

        constexpr std::string_view assignmentSuffix = "Assignment";
        if (name.ends_with(assignmentSuffix)) {
            const std::string_view operatorName = name.substr(0, name.size() - assignmentSuffix.size());
            const bool isCompoundAssignable = operatorName == "addition" || operatorName == "subtraction" || operatorName == "multiplication" || operatorName == "division" ||
                                              operatorName == "integerDivision" || operatorName == "modulo" || operatorName == "exponent" || operatorName == "bitwiseAnd" ||
                                              operatorName == "bitwiseOr" || operatorName == "bitwiseXor" || operatorName == "shiftLeft" || operatorName == "shiftRight" ||
                                              operatorName == "unsignedShiftRight" || operatorName == "nullishCoalescing";
            return isCompoundAssignable && canUseOperator(operatorName);
        }

        if (name == "referenceEquality" || name == "referenceInequality" || name == "instanceOf") return true;
        if (name == "nullishCoalescing") return fullTypeName.starts_with("vanillang.typesystem.Optional<");
        if (name == "logicalAnd" || name == "logicalOr" || name == "logicalNot") return false;

        std::string_view overloadName = name;
        std::optional<bool> overloadHasParameters = std::nullopt;
        if (name == "unaryPlus" || name == "unaryMinus") {
            overloadName = name == "unaryPlus" ? "addition" : "subtraction";
            overloadHasParameters = false;
        } else if (name == "addition" || name == "subtraction") {
            overloadHasParameters = true;
        }

        const std::vector<std::unique_ptr<OperatorDeclarationNode>>* localOperators = nullptr;
        if (const auto* classNode = dynamic_cast<const ClassDeclarationNode*>(localNode)) {
            localOperators = &classNode->getOperatorDeclarations();
        } else if (const auto* interfaceNode = dynamic_cast<const InterfaceDeclarationNode*>(localNode)) {
            localOperators = &interfaceNode->getOperatorDeclarations();
        }

        if (localOperators != nullptr) {
            for (const auto& operatorDecl : *localOperators) {
                if (operatorDecl->getOperatorName() != overloadName) continue;

                const bool hasParameters = !operatorDecl->getParameters().empty();
                if (!overloadHasParameters.has_value() || hasParameters == *overloadHasParameters) return true;
            }
            return false;
        }

        const std::unordered_map<std::string, std::unique_ptr<ImportedOperator>>* importedOperators = nullptr;
        if (const auto* classNode = dynamic_cast<const ImportedClass*>(importedNode)) {
            importedOperators = &classNode->getOperators();
        } else if (const auto* interfaceNode = dynamic_cast<const ImportedInterface*>(importedNode)) {
            importedOperators = &interfaceNode->getOperators();
        }

        if (importedOperators == nullptr) return false;

        constexpr std::string_view internalNamePrefix = "vnl-operator-";
        for (const auto& [internalName, importedOperator] : *importedOperators) {
            std::string_view signature = internalName;
            if (!signature.starts_with(internalNamePrefix)) continue;

            signature.remove_prefix(internalNamePrefix.size());
            const std::size_t parameterListStart = signature.find('(');
            if (parameterListStart == std::string_view::npos || signature.substr(0, parameterListStart) != overloadName) continue;

            const bool hasParameters = signature.substr(parameterListStart) != "()";
            if (!overloadHasParameters.has_value() || hasParameters == *overloadHasParameters) return true;
        }

        return false;
    }

    const std::vector<const Type*> CustomizedType::getGenericArguments() const noexcept {
        return genericArguments;
    }

    const TypeDeclarationNode* CustomizedType::getLocalNode() const noexcept {
        return localNode;
    }

    const ImportedIdentifier* CustomizedType::getImportedNode() const noexcept {
        return importedNode;
    }
} // namespace vnlc
