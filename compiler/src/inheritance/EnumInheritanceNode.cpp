#include "EnumInheritanceNode.hpp"

namespace vnlc {
    EnumInheritanceNode::EnumInheritanceNode(const EnumDeclarationNode* localNode) : InheritanceNode(localNode) {}

    EnumInheritanceNode::EnumInheritanceNode(const ImportedEnum* importedNode) : InheritanceNode(importedNode) {}

    const EnumDeclarationNode* EnumInheritanceNode::getLocalNode() const noexcept {
        return static_cast<const EnumDeclarationNode*>(InheritanceNode::getLocalNode());
    }

    const ImportedEnum* EnumInheritanceNode::getImportedNode() const noexcept {
        return static_cast<const ImportedEnum*>(InheritanceNode::getImportedNode());
    }
} // namespace vnlc
