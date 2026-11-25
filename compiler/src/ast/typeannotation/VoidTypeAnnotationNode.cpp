#include "VoidTypeAnnotationNode.hpp"

namespace vnlc {
    VoidTypeAnnotationNode::VoidTypeAnnotationNode(const Token& firstToken, const Token& lastToken) noexcept : TypeAnnotationNode(false, firstToken, lastToken) {}
} // namespace vnlc
