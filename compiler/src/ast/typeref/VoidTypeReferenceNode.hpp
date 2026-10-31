#ifndef VNLC_VOID_TYPE_REFERENCE_NODE_HPP
#define VNLC_VOID_TYPE_REFERENCE_NODE_HPP

#include "ast/typeref/TypeReferenceNode.hpp"

namespace vnlc {
    class VoidTypeReferenceNode : public TypeReferenceNode {
    public:
        VoidTypeReferenceNode() = delete;

        VoidTypeReferenceNode(const Token& firstToken, const Token& lastToken) noexcept;
    };
} // namespace vnlc

#endif // VNLC_VOID_TYPE_REFERENCE_NODE_HPP
