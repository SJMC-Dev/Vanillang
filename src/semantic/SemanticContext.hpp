#ifndef VNLC_SEMANTIC_CONTEXT_HPP
#define VNLC_SEMANTIC_CONTEXT_HPP

#include "ast/AstNode.hpp"
#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/declaration/EnumDeclarationNode.hpp"
#include "ast/declaration/EnumMemberDeclarationNode.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include "ast/declaration/OperatorDeclarationNode.hpp"
#include "ast/declaration/TypeDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/expression/ExpressionNode.hpp"
#include "ast/module/ModuleNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include "diagnostic/Diagnostic.hpp"
#include "inheritance/ClassInheritanceNode.hpp"
#include "inheritance/EnumInheritanceNode.hpp"
#include "inheritance/EnumMemberInheritanceNode.hpp"
#include "inheritance/InheritanceNode.hpp"
#include "inheritance/InterfaceInheritanceNode.hpp"
#include "scope/Scope.hpp"
#include "semantic/ExpressionAnalysisResult.hpp"
#include "symbol/RegularSymbol.hpp"
#include "type/CustomizedType.hpp"
#include "type/Type.hpp"
#include "vni/import/ImportedClass.hpp"
#include "vni/import/ImportedEnum.hpp"
#include "vni/import/ImportedEnumMember.hpp"
#include "vni/import/ImportedIdentifier.hpp"
#include "vni/import/ImportedInterface.hpp"
#include "vni/import/ImportedPackage.hpp"
#include "vni/import/ImportedRootPackageMap.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace vnlc {
    class SemanticContext {
    private:
        const ModuleNode& module;
        const ImportedRootPackageMap& imports;

        std::vector<Diagnostic> errors;
        std::vector<Diagnostic> warnings;
        std::vector<Diagnostic> notes;

        std::vector<std::unique_ptr<Scope>> scopeStack;
        std::unordered_map<const AstNode*, std::unique_ptr<Scope>> localScopeMap;
        std::unordered_map<const ImportedItem*, std::unique_ptr<Scope>> importedScopeMap;

        std::unordered_map<std::string, std::unique_ptr<CustomizedType>> customizedTypeRegistry;
        std::unordered_map<const TypeReferenceNode*, const Type*> typeMap;
        std::unordered_map<const ValueDeclarationNode*, const Type*> inferredValueTypeMap;
        std::unordered_map<const FunctionDeclarationNode*, const Type*> inferredFunctionReturnTypeMap;
        std::unordered_map<const OperatorDeclarationNode*, const Type*> inferredOperatorReturnTypeMap;

        std::unordered_map<const ExpressionNode*, ExpressionAnalysisResult> expressionAnalysisResultMap;

        std::vector<std::unique_ptr<InheritanceNode>> inheritanceNodes;
        std::unordered_map<const TypeDeclarationNode*, InheritanceNode*> localInheritanceMap;
        std::unordered_map<const ImportedIdentifier*, InheritanceNode*> importedInheritanceMap;

    public:
        SemanticContext(const ModuleNode& module, const ImportedRootPackageMap& imports);
        SemanticContext(const SemanticContext&) = delete;
        SemanticContext(SemanticContext&&) noexcept = default;

        void reportError(const AstNode& node, std::string_view message);
        void reportWarning(const AstNode& node, std::string_view message);
        void reportNote(const AstNode& node, std::string_view message);

        [[nodiscard]] const ModuleNode& getModule() const;
        [[nodiscard]] const ImportedRootPackageMap& getImports() const;

        [[nodiscard]] const ImportedItem* getImportedItemByFullPath(const std::vector<std::string>& fullPath) const;
        [[nodiscard]] const ImportedItem* getImportedItemByFullTypeName(std::string_view fullTypeName) const;
        [[nodiscard]] const ImportedPackage* getImportedRootPackageByName(std::string_view name) const;

        void pushScope(std::unique_ptr<Scope>&& scope);
        bool pushExistingScope(const AstNode* astNode);
        void popScope();
        Scope& getOrCreateImportedScope(ScopeKind kind, const Scope* parent, const ImportedItem& importedNode);

        void registerCustomizedType(std::unique_ptr<CustomizedType>&& customizedType);
        void mapType(const TypeReferenceNode* typeNode, const Type* type);
        void mapInferredValueType(const ValueDeclarationNode* valueDeclaration, const Type* type);
        void mapInferredFunctionReturnType(const FunctionDeclarationNode* functionDeclaration, const Type* type);
        void mapInferredOperatorReturnType(const OperatorDeclarationNode* operatorDeclaration, const Type* type);

        void mapExpressionAnalysisResult(const ExpressionNode* expressionNode, ExpressionAnalysisResult result);

        ClassInheritanceNode& getOrCreateLocalInheritanceNode(const ClassDeclarationNode& localNode);
        InterfaceInheritanceNode& getOrCreateLocalInheritanceNode(const InterfaceDeclarationNode& localNode);
        EnumInheritanceNode& getOrCreateLocalInheritanceNode(const EnumDeclarationNode& localNode);
        EnumMemberInheritanceNode& getOrCreateLocalInheritanceNode(const EnumMemberDeclarationNode& localNode);
        ClassInheritanceNode& getOrCreateImportedInheritanceNode(const ImportedClass& importedNode);
        InterfaceInheritanceNode& getOrCreateImportedInheritanceNode(const ImportedInterface& importedNode);
        EnumInheritanceNode& getOrCreateImportedInheritanceNode(const ImportedEnum& importedNode);
        EnumMemberInheritanceNode& getOrCreateImportedInheritanceNode(const ImportedEnumMember& importedNode);

        [[nodiscard]] const CustomizedType* getCustomizedTypeByFullTypeName(std::string_view fullTypeName) const;
        [[nodiscard]] const Type* getTypeByFullTypeName(std::string_view fullTypeName) const;
        [[nodiscard]] const Type* getTypeByTypeReferenceNode(const TypeReferenceNode* typeNode) const;
        [[nodiscard]] const Type* getInferredValueType(const ValueDeclarationNode& valueDecl) const;
        [[nodiscard]] const Type* getInferredFunctionReturnType(const FunctionDeclarationNode& funcDecl) const;
        [[nodiscard]] const Type* getInferredOperatorReturnType(const OperatorDeclarationNode& opDecl) const;

        [[nodiscard]] const Scope* getScopeByAstNode(const AstNode* astNode) const;
        [[nodiscard]] const Scope* getScopeByImportedNode(const ImportedItem* importedNode) const;
        [[nodiscard]] const Scope* getScopeByRegularSymbol(const RegularSymbol& symbol) const;

        [[nodiscard]] const InheritanceNode* getInheritanceNodeByLocalNode(const TypeDeclarationNode* localNode) const;
        [[nodiscard]] const InheritanceNode* getInheritanceNodeByImportedNode(const ImportedIdentifier* importedNode) const;

        [[nodiscard]] const ExpressionAnalysisResult* getExpressionAnalysisResult(const ExpressionNode* expressionNode) const;

        [[nodiscard]] Scope& currentScope();

        [[nodiscard]] const Scope* currentModuleScope() const noexcept;
        [[nodiscard]] const Scope* currentFunctionScope() const noexcept;
        [[nodiscard]] const Scope* currentClassScope() const noexcept;
        [[nodiscard]] const Scope* currentInterfaceScope() const noexcept;
        [[nodiscard]] const Scope* currentEnumScope() const noexcept;
        [[nodiscard]] const Scope* currentEnumMemberScope() const noexcept;
        [[nodiscard]] const Scope* currentBlockScope() const noexcept;
        [[nodiscard]] const Scope* currentLoopScope() const noexcept;
        [[nodiscard]] const Scope* currentSwitchScope() const noexcept;

        [[nodiscard]] const std::vector<Diagnostic>& getErrors() const noexcept;
        [[nodiscard]] const std::vector<Diagnostic>& getWarnings() const noexcept;
        [[nodiscard]] const std::vector<Diagnostic>& getNotes() const noexcept;

        [[nodiscard]] std::tuple<std::vector<Diagnostic>, std::vector<Diagnostic>, std::vector<Diagnostic>> takeDiagnostics();
        [[nodiscard]] std::unordered_map<const AstNode*, std::unique_ptr<Scope>> takeLocalScopeMap();
        [[nodiscard]] std::unordered_map<const ImportedItem*, std::unique_ptr<Scope>> takeImportedScopeMap();
        [[nodiscard]] std::unordered_map<std::string, std::unique_ptr<CustomizedType>> takeCustomizedTypeRegistry();
        [[nodiscard]] std::unordered_map<const TypeReferenceNode*, const Type*> takeTypeMap();
        [[nodiscard]] std::unordered_map<const ValueDeclarationNode*, const Type*> takeInferredValueTypeMap();
        [[nodiscard]] std::unordered_map<const FunctionDeclarationNode*, const Type*> takeInferredFunctionReturnTypeMap();
        [[nodiscard]] std::unordered_map<const OperatorDeclarationNode*, const Type*> takeInferredOperatorReturnTypeMap();
        [[nodiscard]] std::unordered_map<const ExpressionNode*, ExpressionAnalysisResult> takeExpressionAnalysisResultMap();
        [[nodiscard]] std::vector<std::unique_ptr<InheritanceNode>> takeInheritanceNodes();
        [[nodiscard]] std::unordered_map<const TypeDeclarationNode*, InheritanceNode*> takeLocalInheritanceMap();
        [[nodiscard]] std::unordered_map<const ImportedIdentifier*, InheritanceNode*> takeImportedInheritanceMap();
    };
} // namespace vnlc

#endif // VNLC_SEMANTIC_CONTEXT_HPP
