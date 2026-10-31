#ifndef VNLC_INTERFACE_INHERITANCE_NODE_HPP
#define VNLC_INTERFACE_INHERITANCE_NODE_HPP

#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include "inheritance/InheritanceNode.hpp"
#include "vni/import/ImportedInterface.hpp"

namespace vnlc {
    class InterfaceInheritanceNode : public InheritanceNode {
    public:
        InterfaceInheritanceNode(const InterfaceDeclarationNode* localNode);
        InterfaceInheritanceNode(const ImportedInterface* importedNode);

        InterfaceInheritanceNode() = delete;
        InterfaceInheritanceNode(const InterfaceInheritanceNode&) = delete;
        InterfaceInheritanceNode(InterfaceInheritanceNode&&) noexcept = default;

        [[nodiscard]] const InterfaceDeclarationNode* getLocalNode() const noexcept override;
        [[nodiscard]] const ImportedInterface* getImportedNode() const noexcept override;
    };
} // namespace vnlc

#endif // VNLC_INTERFACE_INHERITANCE_NODE_HPP
