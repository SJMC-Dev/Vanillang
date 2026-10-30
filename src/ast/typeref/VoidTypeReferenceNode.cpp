#include "VoidTypeReferenceNode.hpp"

namespace vnlc {
    VoidTypeReferenceNode::VoidTypeReferenceNode(const Token& firstToken, const Token& lastToken) noexcept : TypeReferenceNode(false, firstToken, lastToken) {}
} // namespace vnlc
