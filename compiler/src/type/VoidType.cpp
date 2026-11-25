#include "VoidType.hpp"
#include <string_view>

namespace vnlc {
    std::string_view VoidType::getFullTypeName() const noexcept {
        return "void";
    }

    bool VoidType::canConvertTo(const Type& other) const noexcept {
        return dynamic_cast<const VoidType*>(&other) != nullptr;
    }

    const VoidType* VoidType::voidType() {
        static const VoidType resolvedVoidType{};
        return &resolvedVoidType;
    }
} // namespace vnlc
