#ifndef VNLC_INHERITANCE_NODE_HPP
#define VNLC_INHERITANCE_NODE_HPP

#include "ast/declaration/TypeDeclarationNode.hpp"
#include "inheritance/InheritanceOrigin.hpp"
#include "vni/import/ImportedIdentifier.hpp"

namespace vnlc {
    class InheritanceNode {
    private:
        InheritanceOrigin origin;
        const TypeDeclarationNode* localNode;
        const ImportedIdentifier* importedNode;

    protected:
        InheritanceNode(const TypeDeclarationNode* localNode);
        InheritanceNode(const ImportedIdentifier* importedNode);

    public:
        InheritanceNode() = delete;
        InheritanceNode(const InheritanceNode&) = delete;
        InheritanceNode(InheritanceNode&&) noexcept = default;

        virtual ~InheritanceNode() = default;

        [[nodiscard]] const InheritanceOrigin getOrigin() const noexcept;
        [[nodiscard]] virtual const TypeDeclarationNode* getLocalNode() const noexcept;
        [[nodiscard]] virtual const ImportedIdentifier* getImportedNode() const noexcept;
        [[nodiscard]] virtual const bool hasCycle() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_INHERITANCE_NODE_HPP
