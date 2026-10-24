#ifndef VNLC_SEMANTIC_ANALYZER_HPP
#define VNLC_SEMANTIC_ANALYZER_HPP

#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/declaration/ConstructorDeclarationNode.hpp"
#include "ast/declaration/DeclarationNode.hpp"
#include "ast/declaration/EnumDeclarationNode.hpp"
#include "ast/declaration/EnumMemberDeclarationNode.hpp"
#include "ast/declaration/ExportDeclarationNode.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/ImportDeclarationNode.hpp"
#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include "ast/declaration/OperatorDeclarationNode.hpp"
#include "ast/declaration/TypeAliasDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/expression/ExpressionNode.hpp"
#include "ast/module/ModuleNode.hpp"
#include "ast/statement/StatementNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include "config/Config.hpp"
#include "semantic/SemanticContext.hpp"
#include "semantic/SemanticResult.hpp"
#include "semantic/checkingresults/CheckingResults.hpp"
#include "semantic/collectingresults/CollectingResults.hpp"
#include "symbol/RegularSymbolAccessModifier.hpp"
#include "symbol/RegularSymbolKind.hpp"
#include "type/Type.hpp"
#include "vni/import/ImportedItem.hpp"
#include "vni/import/ImportedModule.hpp"
#include "vni/import/ImportedPackage.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace vnlc {
    class PrimitiveType;

    class SemanticAnalyzer {
        friend class SemanticAnalyzerImportTest;

    private:
        const ModuleNode& module;
        const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& imports;
        SemanticContext context;

        [[nodiscard]] static RegularSymbolKind getImportedSymbolKind(const ImportedItem& item);
        [[nodiscard]] static std::optional<ScopeKind> getImportedScopeKind(const ImportedItem& item);
        [[nodiscard]] static RegularSymbolAccessModifier getImportedAccessModifier(const ImportedItem& item);

        [[nodiscard]] const ImportedPackage* getImportedRootPackageByName(std::string_view name) const;

        [[nodiscard]] LocalFunctionSymbolsInModuleCollectingResult
        declareFunctionOverloading(const FunctionDeclarationNode& funcDecl, RegularSymbolKind kind, std::string_view declarationKind);
        const Type* registerImportedTypeReference(std::string_view fullTypeName);
        void registerImportedTypeReferences(const ImportedItem& importedItem);
        void registerImportedScopes(const ImportedItem& item, const Scope* parent);

        [[nodiscard]] LocalRegularSymbolsInModuleCollectingResult collectLocalRegularSymbolsInModule(const ModuleNode& moduleNode);
        [[nodiscard]] LocalRegularSymbolsInFunctionCollectingResult collectLocalRegularSymbolsInFunction(const FunctionDeclarationNode& functionNode);
        [[nodiscard]] LocalRegularSymbolsInConstructorCollectingResult collectLocalRegularSymbolsInConstructor(const ConstructorDeclarationNode& constructorNode);
        [[nodiscard]] LocalRegularSymbolsInOperatorCollectingResult collectLocalRegularSymbolsInOperator(const OperatorDeclarationNode& operatorNode);
        [[nodiscard]] LocalRegularSymbolsInClassCollectingResult collectLocalRegularSymbolsInClass(const ClassDeclarationNode& classNode);
        [[nodiscard]] LocalRegularSymbolsInInterfaceCollectingResult collectLocalRegularSymbolsInInterface(const InterfaceDeclarationNode& interfaceNode);
        [[nodiscard]] LocalRegularSymbolsInEnumCollectingResult collectLocalRegularSymbolsInEnum(const EnumDeclarationNode& enumNode);
        [[nodiscard]] LocalRegularSymbolsInEnumMemberCollectingResult collectLocalRegularSymbolsInEnumMember(const EnumMemberDeclarationNode& enumMemberNode);
        [[nodiscard]] LocalRegularSymbolsInTypeAliasCollectingResult collectLocalRegularSymbolsInTypeAlias(const TypeAliasDeclarationNode& typeAliasNode);
        [[nodiscard]] LocalFunctionSymbolsInModuleCollectingResult collectLocalFunctionSymbolsInModule(const ModuleNode& moduleNode);
        [[nodiscard]] LocalFunctionSymbolsInClassCollectingResult collectLocalFunctionSymbolsInClass(const ClassDeclarationNode& classNode);
        [[nodiscard]] LocalFunctionSymbolsInInterfaceCollectingResult collectLocalFunctionSymbolsInInterface(const InterfaceDeclarationNode& interfaceNode);

        [[nodiscard]] ModuleCheckingResult generateInternalName(const DeclarationNode& node);
        [[nodiscard]] ModuleCheckingResult generateInternalNameForOverloadablesInModule(const ModuleNode& moduleNode);
        [[nodiscard]] ModuleCheckingResult generateInternalNameForOverloadablesInClass(const ClassDeclarationNode& classNode);
        [[nodiscard]] ModuleCheckingResult generateInternalNameForOverloadablesInInterface(const InterfaceDeclarationNode& interfaceNode);

        [[nodiscard]] ModuleCheckingResult buildInheritanceGraph(const ModuleNode& localModule, const std::vector<const ImportedModule*>& importedModules);

        [[nodiscard]] MetadataCheckingResult checkMetadata(const std::vector<DeclarationItem::MetadataTerm>& metadataTerms, const DeclarationNode& declNode);
        [[nodiscard]] ModuleCheckingResult checkModule(const ModuleNode& moduleNode, const Config& config);
        [[nodiscard]] ImportCheckingResult checkImport(const ImportDeclarationNode& importDecl, const Config& config);
        [[nodiscard]] ExportCheckingResult checkExport(const ExportDeclarationNode& exportDecl);
        [[nodiscard]] ValueDeclarationCheckingResult checkValueDeclaration(const ValueDeclarationNode& varDecl);
        [[nodiscard]] FunctionDeclarationCheckingResult checkFunctionDeclaration(const FunctionDeclarationNode& funcDecl);
        [[nodiscard]] ConstructorDeclarationCheckingResult checkConstructorDeclaration(const ConstructorDeclarationNode& constructorDecl);
        [[nodiscard]] OperatorDeclarationCheckingResult checkOperatorDeclaration(const OperatorDeclarationNode& operatorDecl);
        [[nodiscard]] ClassDeclarationCheckingResult checkClassDeclaration(const ClassDeclarationNode& classDecl, const Config& config);
        [[nodiscard]] InterfaceDeclarationCheckingResult checkInterfaceDeclaration(const InterfaceDeclarationNode& interfaceDecl, const Config& config);
        [[nodiscard]] EnumDeclarationCheckingResult checkEnumDeclaration(const EnumDeclarationNode& enumDecl, const Config& config);
        [[nodiscard]] TypeAliasDeclarationCheckingResult checkTypeAliasDeclaration(const TypeAliasDeclarationNode& typeAliasDecl, const Config& config);
        [[nodiscard]] StatementCheckingResult checkStatement(const StatementNode& statement);
        [[nodiscard]] ExpressionCheckingResult checkExpression(const ExpressionNode& expression);
        [[nodiscard]] TypeCheckingResult checkType(const TypeReferenceNode& type);

    public:
        explicit SemanticAnalyzer(const ModuleNode& module);
        SemanticAnalyzer(const ModuleNode& module, const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& imports);

        SemanticAnalyzer() = delete;
        SemanticAnalyzer(const SemanticAnalyzer&) = delete;
        SemanticAnalyzer& operator=(const SemanticAnalyzer&) = delete;

        SemanticAnalyzer(SemanticAnalyzer&&) noexcept = default;
        SemanticAnalyzer& operator=(SemanticAnalyzer&&) noexcept = delete;

        [[nodiscard]] SemanticResult analyze(const Config& config);
    };
} // namespace vnlc

#endif // VNLC_SEMANTIC_ANALYZER_HPP
