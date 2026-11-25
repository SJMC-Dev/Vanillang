#pragma once

#include "type/Type.hpp"
#include <string_view>

namespace vnlc {
    class VoidType : public Type {
    private:
        VoidType() = default;

    public:
        VoidType(const VoidType&) = delete;
        VoidType(VoidType&&) noexcept = delete;

        [[nodiscard]] std::string_view getFullTypeName() const noexcept override;
        [[nodiscard]] bool canConvertTo(const Type& other) const noexcept override;

        [[nodiscard]] static const VoidType* voidType();
    };
} // namespace vnlc
