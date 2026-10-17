#ifndef VNLC_CLASS_INHERITANCE_NODE_HPP
#define VNLC_CLASS_INHERITANCE_NODE_HPP

#include "ast/declaration/ClassDeclarationNode.hpp"
#include "inheritance/ClassInheritanceOrigin.hpp"
#include "vni/import/ImportedClass.hpp"

namespace vnlc {
    class ClassInheritanceNode {
    private:
        ClassInheritanceOrigin origin;
        const ClassDeclarationNode* localNode;
        const ImportedClass* importedNode;
        const ClassInheritanceNode* parent;

    public:
        ClassInheritanceNode(const ClassDeclarationNode* localNode);
        ClassInheritanceNode(const ImportedClass* importedNode);

        ClassInheritanceNode() = delete;
        ClassInheritanceNode(const ClassInheritanceNode&) = delete;
        ClassInheritanceNode(ClassInheritanceNode&&) noexcept = default;

        [[nodiscard]] const ClassInheritanceOrigin getOrigin() const noexcept;
        [[nodiscard]] const ClassInheritanceNode* findParent() const noexcept;
        [[nodiscard]] const ClassDeclarationNode* getLocalNode() const noexcept;
        [[nodiscard]] const ImportedClass* getImportedNode() const noexcept;
        [[nodiscard]] const bool hasCycle() const noexcept;

        void setParent(const ClassInheritanceNode* parent) noexcept;
    };
} // namespace vnlc

#endif // VNLC_CLASS_INHERITANCE_NODE_HPP
