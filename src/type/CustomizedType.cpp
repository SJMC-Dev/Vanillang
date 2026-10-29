#include "CustomizedType.hpp"
#include "type/CustomizedTypeOrigin.hpp"
#include "type/PrimitiveType.hpp"

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
