#include "InheritanceNode.hpp"

namespace vnlc {
    InheritanceNode::InheritanceNode(const TypeDeclarationNode* localNode) : origin(InheritanceOrigin::LOCAL), localNode(localNode), importedNode(nullptr) {}

    InheritanceNode::InheritanceNode(const ImportedIdentifier* importedNode) : origin(InheritanceOrigin::IMPORTED), localNode(nullptr), importedNode(importedNode) {}

    InheritanceOrigin InheritanceNode::getOrigin() const noexcept {
        return origin;
    }

    const TypeDeclarationNode* InheritanceNode::getLocalNode() const noexcept {
        return localNode;
    }

    const ImportedIdentifier* InheritanceNode::getImportedNode() const noexcept {
        return importedNode;
    }

    const InheritanceNode* InheritanceNode::findParent() const noexcept {
        return nullptr;
    }

    bool InheritanceNode::hasCycle() const noexcept {
        return false;
    }
} // namespace vnlc
