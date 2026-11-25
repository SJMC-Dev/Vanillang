#pragma once

#include "ast/declaration/EnumMemberDeclarationNode.hpp"
#include "inheritance/EnumInheritanceNode.hpp"
#include "inheritance/InheritanceNode.hpp"
#include "vni/import/ImportedEnumMember.hpp"

namespace vnlc {
    class EnumMemberInheritanceNode : public InheritanceNode {
    private:
        const EnumInheritanceNode* parent;

    public:
        EnumMemberInheritanceNode(const EnumMemberDeclarationNode* localNode);
        EnumMemberInheritanceNode(const ImportedEnumMember* importedNode);

        EnumMemberInheritanceNode() = delete;
        EnumMemberInheritanceNode(const EnumMemberInheritanceNode&) = delete;
        EnumMemberInheritanceNode(EnumMemberInheritanceNode&&) noexcept = default;

        [[nodiscard]] const EnumMemberDeclarationNode* getLocalNode() const noexcept override;
        [[nodiscard]] const ImportedEnumMember* getImportedNode() const noexcept override;
        [[nodiscard]] const EnumInheritanceNode* findParent() const noexcept override;

        void setParent(const EnumInheritanceNode* parent) noexcept;
    };
} // namespace vnlc
