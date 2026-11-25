#include "EnumMemberInheritanceNode.hpp"

namespace vnlc {
    EnumMemberInheritanceNode::EnumMemberInheritanceNode(const EnumMemberDeclarationNode* localNode) : InheritanceNode(localNode), parent(nullptr) {}

    EnumMemberInheritanceNode::EnumMemberInheritanceNode(const ImportedEnumMember* importedNode) : InheritanceNode(importedNode), parent(nullptr) {}

    const EnumMemberDeclarationNode* EnumMemberInheritanceNode::getLocalNode() const noexcept {
        return static_cast<const EnumMemberDeclarationNode*>(InheritanceNode::getLocalNode());
    }

    const ImportedEnumMember* EnumMemberInheritanceNode::getImportedNode() const noexcept {
        return static_cast<const ImportedEnumMember*>(InheritanceNode::getImportedNode());
    }

    const EnumInheritanceNode* EnumMemberInheritanceNode::findParent() const noexcept {
        return parent;
    }

    void EnumMemberInheritanceNode::setParent(const EnumInheritanceNode* parent) noexcept {
        this->parent = parent;
    }
} // namespace vnlc
