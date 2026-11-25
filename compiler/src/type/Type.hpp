#pragma once

#include <string_view>

namespace vnlc {
    class Type {
    public:
        [[nodiscard]] virtual std::string_view getFullTypeName() const noexcept = 0;
        [[nodiscard]] virtual bool canConvertTo(const Type& other) const noexcept = 0;

        virtual ~Type() = default;
    };
} // namespace vnlc
