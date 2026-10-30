#ifndef VNLC_VOID_TYPE_HPP
#define VNLC_VOID_TYPE_HPP

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

#endif // VNLC_VOID_TYPE_HPP
