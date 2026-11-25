#include "InterfaceInheritanceNode.hpp"

namespace vnlc {
    InterfaceInheritanceNode::InterfaceInheritanceNode(const InterfaceDeclarationNode* localNode) : InheritanceNode(localNode) {}

    InterfaceInheritanceNode::InterfaceInheritanceNode(const ImportedInterface* importedNode) : InheritanceNode(importedNode) {}

    const InterfaceDeclarationNode* InterfaceInheritanceNode::getLocalNode() const noexcept {
        return static_cast<const InterfaceDeclarationNode*>(InheritanceNode::getLocalNode());
    }

    const ImportedInterface* InterfaceInheritanceNode::getImportedNode() const noexcept {
        return static_cast<const ImportedInterface*>(InheritanceNode::getImportedNode());
    }
} // namespace vnlc
