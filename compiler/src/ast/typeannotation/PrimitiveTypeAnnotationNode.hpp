#ifndef VNLC_PRIMITIVE_TYPE_ANNOTATION_NODE_HPP
#define VNLC_PRIMITIVE_TYPE_ANNOTATION_NODE_HPP

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

#endif // VNLC_PRIMITIVE_TYPE_ANNOTATION_NODE_HPP
