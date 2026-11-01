#ifndef VNLC_VOID_TYPE_ANNOTATION_NODE_HPP
#define VNLC_VOID_TYPE_ANNOTATION_NODE_HPP

#include "ast/typeannotation/TypeAnnotationNode.hpp"

namespace vnlc {
    class VoidTypeAnnotationNode : public TypeAnnotationNode {
    public:
        VoidTypeAnnotationNode() = delete;

        VoidTypeAnnotationNode(const Token& firstToken, const Token& lastToken) noexcept;
    };
} // namespace vnlc

#endif // VNLC_VOID_TYPE_ANNOTATION_NODE_HPP
