#ifndef VNLC_EXPORT_DECLARATION_NODE_HPP
#define VNLC_EXPORT_DECLARATION_NODE_HPP

#include "ast/declaration/DeclarationNode.hpp"
#include "ast/declaration/ExportDeclarationItem.hpp"
#include <vector>

namespace vnlc {
    class ExportDeclarationNode : public DeclarationNode {
    private:
        std::vector<ExportDeclarationItem> nameList;

    public:
        ExportDeclarationNode() = delete;

        ExportDeclarationNode(std::vector<ExportDeclarationItem>&& nameList, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const std::vector<ExportDeclarationItem>& getNameList() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_EXPORT_DECLARATION_NODE_HPP