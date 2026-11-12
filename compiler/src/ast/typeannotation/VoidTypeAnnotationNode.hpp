#pragma once

#include "ast/typeannotation/TypeAnnotationNode.hpp"

namespace vnlc {
    class VoidTypeAnnotationNode : public TypeAnnotationNode {
    public:
        VoidTypeAnnotationNode() = delete;

        VoidTypeAnnotationNode(const Token& firstToken, const Token& lastToken) noexcept;
    };
} // namespace vnlc
