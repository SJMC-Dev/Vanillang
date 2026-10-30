#include "ClassInheritanceNode.hpp"

namespace vnlc {
    ClassInheritanceNode::ClassInheritanceNode(const ClassDeclarationNode* localNode) : InheritanceNode(localNode), parent(nullptr) {}

    ClassInheritanceNode::ClassInheritanceNode(const ImportedClass* importedNode) : InheritanceNode(importedNode), parent(nullptr) {}

    const ClassDeclarationNode* ClassInheritanceNode::getLocalNode() const noexcept {
        return static_cast<const ClassDeclarationNode*>(InheritanceNode::getLocalNode());
    }

    const ImportedClass* ClassInheritanceNode::getImportedNode() const noexcept {
        return static_cast<const ImportedClass*>(InheritanceNode::getImportedNode());
    }

    bool ClassInheritanceNode::hasCycle() const noexcept {
        const ClassInheritanceNode* slow = this;
        const ClassInheritanceNode* fast = this;
        while (fast != nullptr && fast->parent != nullptr) {
            slow = slow->parent;
            fast = fast->parent->parent;
            if (slow == fast) return true;
        }
        return false;
    }

    const ClassInheritanceNode* ClassInheritanceNode::findParent() const noexcept {
        return parent;
    }

    const std::vector<const InterfaceInheritanceNode*>& ClassInheritanceNode::getImplementedInterfaces() const noexcept {
        return implementedInterfaces;
    }

    void ClassInheritanceNode::setParent(const ClassInheritanceNode* parent) noexcept {
        this->parent = parent;
    }

    void ClassInheritanceNode::addImplementedInterface(const InterfaceInheritanceNode* implementedInterface) {
        implementedInterfaces.push_back(implementedInterface);
    }
} // namespace vnlc
