#ifndef VNLC_PRIMITIVE_TYPE_HPP
#define VNLC_PRIMITIVE_TYPE_HPP

#include "type/PrimitiveTypeKind.hpp"
#include "type/Type.hpp"
#include <string_view>

namespace vnlc {
    class PrimitiveType : public Type {
    private:
        PrimitiveTypeKind primitiveKind;

        PrimitiveType(PrimitiveTypeKind primitiveKind);

    public:
        PrimitiveType(const PrimitiveType&) = delete;
        PrimitiveType(PrimitiveType&&) noexcept = delete;

        [[nodiscard]] PrimitiveTypeKind getPrimitiveKind() const noexcept;
        [[nodiscard]] std::string_view getFullTypeName() const noexcept override;
        [[nodiscard]] bool canConvertTo(const Type& other) const noexcept override;

        [[nodiscard]] static const PrimitiveType* byteType();
        [[nodiscard]] static const PrimitiveType* shortType();
        [[nodiscard]] static const PrimitiveType* intType();
        [[nodiscard]] static const PrimitiveType* longType();
        [[nodiscard]] static const PrimitiveType* floatType();
        [[nodiscard]] static const PrimitiveType* doubleType();
        [[nodiscard]] static const PrimitiveType* booleanType();
        [[nodiscard]] static const PrimitiveType* stringType();
    };
} // namespace vnlc

#endif // VNLC_PRIMITIVE_TYPE_HPP
