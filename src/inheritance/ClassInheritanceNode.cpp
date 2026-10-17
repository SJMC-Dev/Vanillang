#include "ClassInheritanceNode.hpp"

namespace vnlc {
    ClassInheritanceNode::ClassInheritanceNode(const ClassDeclarationNode* localNode) : origin(ClassInheritanceOrigin::LOCAL), localNode(localNode), importedNode(nullptr), parent(nullptr) {}

    ClassInheritanceNode::ClassInheritanceNode(const ImportedClass* importedNode) : origin(ClassInheritanceOrigin::IMPORTED), localNode(nullptr), importedNode(importedNode), parent(nullptr) {}

    const ClassInheritanceOrigin ClassInheritanceNode::getOrigin() const noexcept {
        return origin;
    }

    const ClassInheritanceNode* ClassInheritanceNode::findParent() const noexcept {
        return parent;
    }

    const ClassDeclarationNode* ClassInheritanceNode::getLocalNode() const noexcept {
        return localNode;
    }

    const ImportedClass* ClassInheritanceNode::getImportedNode() const noexcept {
        return importedNode;
    }

    const bool ClassInheritanceNode::hasCycle() const noexcept {
        const ClassInheritanceNode* slow = this;
        const ClassInheritanceNode* fast = this;
        while (fast != nullptr && fast->parent != nullptr) {
            slow = slow->parent;
            fast = fast->parent->parent;
            if (slow == fast) return true;
        }
        return false;
    }

    void ClassInheritanceNode::setParent(const ClassInheritanceNode* parent) noexcept {
        this->parent = parent;
    }
} // namespace vnlc
