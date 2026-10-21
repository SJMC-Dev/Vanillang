#ifndef VNLC_ENUM_INHERITANCE_NODE_HPP
#define VNLC_ENUM_INHERITANCE_NODE_HPP

#include "ast/declaration/EnumDeclarationNode.hpp"
#include "inheritance/InheritanceNode.hpp"
#include "vni/import/ImportedEnum.hpp"

namespace vnlc {
    class EnumInheritanceNode : public InheritanceNode {
    public:
        EnumInheritanceNode(const EnumDeclarationNode* localNode);
        EnumInheritanceNode(const ImportedEnum* importedNode);

        EnumInheritanceNode() = delete;
        EnumInheritanceNode(const EnumInheritanceNode&) = delete;
        EnumInheritanceNode(EnumInheritanceNode&&) noexcept = default;

        [[nodiscard]] const EnumDeclarationNode* getLocalNode() const noexcept override;
        [[nodiscard]] const ImportedEnum* getImportedNode() const noexcept override;
    };
} // namespace vnlc

#endif // VNLC_ENUM_INHERITANCE_NODE_HPP
