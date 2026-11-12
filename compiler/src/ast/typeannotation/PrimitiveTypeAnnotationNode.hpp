#pragma once

#include "ast/typeannotation/PrimitiveTypeAnnotationKind.hpp"
#include "ast/typeannotation/TypeAnnotationNode.hpp"
#include <string_view>

namespace vnlc {
    class PrimitiveTypeAnnotationNode : public TypeAnnotationNode {
    private:
        PrimitiveTypeAnnotationKind kind;

    public:
        PrimitiveTypeAnnotationNode() = delete;

        PrimitiveTypeAnnotationNode(PrimitiveTypeAnnotationKind kind, bool questionMarkSuffix, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] PrimitiveTypeAnnotationKind getKind() const noexcept;
        [[nodiscard]] std::string_view getPrimitiveTypeName() const noexcept;
    };
} // namespace vnlc
