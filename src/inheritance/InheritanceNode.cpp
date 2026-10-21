#include "InheritanceNode.hpp"

namespace vnlc {
    InheritanceNode::InheritanceNode(const TypeDeclarationNode* localNode) : origin(InheritanceOrigin::LOCAL), localNode(localNode), importedNode(nullptr) {}

    InheritanceNode::InheritanceNode(const ImportedIdentifier* importedNode) : origin(InheritanceOrigin::IMPORTED), localNode(nullptr), importedNode(importedNode) {}

    const InheritanceOrigin InheritanceNode::getOrigin() const noexcept {
        return origin;
    }

    const TypeDeclarationNode* InheritanceNode::getLocalNode() const noexcept {
        return localNode;
    }

    const ImportedIdentifier* InheritanceNode::getImportedNode() const noexcept {
        return importedNode;
    }

    const bool InheritanceNode::hasCycle() const noexcept {
        return false;
    }
} // namespace vnlc
