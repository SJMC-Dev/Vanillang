#ifndef VNLC_CLASS_INHERITANCE_NODE_HPP
#define VNLC_CLASS_INHERITANCE_NODE_HPP

#include "ast/declaration/ClassDeclarationNode.hpp"
#include "inheritance/InheritanceNode.hpp"
#include "inheritance/InterfaceInheritanceNode.hpp"
#include "vni/import/ImportedClass.hpp"
#include <vector>

namespace vnlc {
    class ClassInheritanceNode : public InheritanceNode {
    private:
        const ClassInheritanceNode* parent;
        std::vector<const InterfaceInheritanceNode*> implementedInterfaces;

    public:
        ClassInheritanceNode(const ClassDeclarationNode* localNode);
        ClassInheritanceNode(const ImportedClass* importedNode);

        ClassInheritanceNode() = delete;
        ClassInheritanceNode(const ClassInheritanceNode&) = delete;
        ClassInheritanceNode(ClassInheritanceNode&&) noexcept = default;

        [[nodiscard]] const ClassDeclarationNode* getLocalNode() const noexcept override;
        [[nodiscard]] const ImportedClass* getImportedNode() const noexcept override;
        [[nodiscard]] const bool hasCycle() const noexcept override;
        [[nodiscard]] const ClassInheritanceNode* findParent() const noexcept;
        [[nodiscard]] const std::vector<const InterfaceInheritanceNode*>& getImplementedInterfaces() const noexcept;

        void setParent(const ClassInheritanceNode* parent) noexcept;
        void addImplementedInterface(const InterfaceInheritanceNode* implementedInterface);
    };
} // namespace vnlc

#endif // VNLC_CLASS_INHERITANCE_NODE_HPP
