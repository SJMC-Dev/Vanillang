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
#include "ast/expression/BinaryExpressionNode.hpp"
#include "ast/expression/ConditionalExpressionNode.hpp"
#include "ast/expression/DictLiteralExpressionNode.hpp"
#include "ast/expression/ExpressionNode.hpp"
#include "ast/expression/FunctionCallExpressionNode.hpp"
#include "ast/expression/IdentifierLikeExpressionNode.hpp"
#include "ast/expression/ListLikeLiteralExpressionNode.hpp"
#include "ast/expression/MemberAccessExpressionNode.hpp"
#include "ast/expression/NoneExpressionNode.hpp"
#include "ast/expression/PrimitiveTypeExpressionNode.hpp"
#include "ast/expression/RangeExpressionNode.hpp"
#include "ast/expression/SelectorLiteralExpressionNode.hpp"
#include "ast/expression/SimpleLiteralExpressionNode.hpp"
#include "ast/expression/StringLiteralExpressionNode.hpp"
#include "ast/expression/SubscriptExpressionNode.hpp"
#include "ast/expression/SuperExpressionNode.hpp"
#include "ast/expression/ThisExpressionNode.hpp"
#include "ast/expression/UnaryExpressionNode.hpp"
#include "ast/module/ModuleNode.hpp"
#include "ast/statement/BlockStatementNode.hpp"
#include "ast/statement/BreakStatementNode.hpp"
#include "ast/statement/ContinueStatementNode.hpp"
#include "ast/statement/ExpressionStatementNode.hpp"
#include "ast/statement/ForStatementNode.hpp"
#include "ast/statement/IfStatementNode.hpp"
#include "ast/statement/ReloadStatementNode.hpp"
#include "ast/statement/ReturnStatementNode.hpp"
#include "ast/statement/StatementNode.hpp"
#include "ast/statement/SwitchStatementNode.hpp"
#include "ast/statement/VariableDeclarationStatementNode.hpp"
#include "ast/statement/WhileStatementNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include "config/Config.hpp"
#include "metadata/MetadataInfo.hpp"
#include "semantic/SemanticContext.hpp"
#include "semantic/SemanticResult.hpp"
#include "symbol/RegularSymbolKind.hpp"
#include "vni/import/ImportedItem.hpp"
#include "vni/import/ImportedModule.hpp"
#include <string_view>
#include <vector>

namespace vnlc {
    class PrimitiveType;

    class SemanticAnalyzer {
        friend class SemanticAnalyzerImportTest;

    private:
        SemanticContext context;

        template <typename CallableDeclarationNode>
        const Type* inferReturnType(const CallableDeclarationNode& node);

        void declareFunctionOverloading(const FunctionDeclarationNode& funcDecl, RegularSymbolKind kind, std::string_view declarationKind);

        void registerImportedTypeReference(std::string_view fullTypeName);
        void registerImportedTypeReferences(const ImportedItem& importedItem);
        void registerImportedScopes(const ImportedItem& item, const Scope* parent);

        void collectLocalRegularSymbolsInModule(const ModuleNode& moduleNode);
        void collectLocalRegularSymbolsInFunction(const FunctionDeclarationNode& functionNode);
        void collectLocalRegularSymbolsInConstructor(const ConstructorDeclarationNode& constructorNode);
        void collectLocalRegularSymbolsInOperator(const OperatorDeclarationNode& operatorNode);
        void collectLocalRegularSymbolsInClass(const ClassDeclarationNode& classNode);
        void collectLocalRegularSymbolsInInterface(const InterfaceDeclarationNode& interfaceNode);
        void collectLocalRegularSymbolsInEnum(const EnumDeclarationNode& enumNode);
        void collectLocalRegularSymbolsInEnumMember(const EnumMemberDeclarationNode& enumMemberNode);
        void collectLocalRegularSymbolsInTypeAlias(const TypeAliasDeclarationNode& typeAliasNode);

        void resolveTypeReference(const TypeReferenceNode& typeRef);
        void resolveTypeReferencesInModule(const ModuleNode& moduleNode);
        void resolveTypeReferencesInFunction(const FunctionDeclarationNode& functionNode);
        void resolveTypeReferencesInConstructor(const ConstructorDeclarationNode& constructorNode);
        void resolveTypeReferencesInOperator(const OperatorDeclarationNode& operatorNode);
        void resolveTypeReferencesInClass(const ClassDeclarationNode& classNode);
        void resolveTypeReferencesInInterface(const InterfaceDeclarationNode& interfaceNode);
        void resolveTypeReferencesInEnum(const EnumDeclarationNode& enumNode);
        void resolveTypeReferencesInEnumMember(const EnumMemberDeclarationNode& enumMemberNode);
        void resolveTypeReferencesInTypeAlias(const TypeAliasDeclarationNode& typeAliasNode);

        void generateInternalName(const DeclarationNode& node);
        void generateInternalNameForOverloadablesInModule(const ModuleNode& moduleNode);
        void generateInternalNameForOverloadablesInClass(const ClassDeclarationNode& classNode);
        void generateInternalNameForOverloadablesInInterface(const InterfaceDeclarationNode& interfaceNode);

        void collectLocalFunctionSymbolsInModule(const ModuleNode& moduleNode);
        void collectLocalFunctionSymbolsInClass(const ClassDeclarationNode& classNode);
        void collectLocalFunctionSymbolsInInterface(const InterfaceDeclarationNode& interfaceNode);

        void buildInheritanceGraph(const ModuleNode& localModule, const std::vector<const ImportedModule*>& importedModules);

        MetadataInfo checkMetadata(const std::vector<DeclarationItem::MetadataTerm>& metadataTerms, const DeclarationNode& declNode);
        void checkModule(const ModuleNode& moduleNode, const Config& config);
        void checkImport(const ImportDeclarationNode& importDecl, const Config& config);
        void checkExport(const ExportDeclarationNode& exportDecl);
        void checkValueDeclaration(const ValueDeclarationNode& varDecl);
        void checkFunctionDeclaration(const FunctionDeclarationNode& funcDecl);
        void checkConstructorDeclaration(const ConstructorDeclarationNode& constructorDecl);
        void checkOperatorDeclaration(const OperatorDeclarationNode& operatorDecl);
        void checkClassDeclaration(const ClassDeclarationNode& classDecl, const Config& config);
        void checkInterfaceDeclaration(const InterfaceDeclarationNode& interfaceDecl, const Config& config);
        void checkEnumDeclaration(const EnumDeclarationNode& enumDecl, const Config& config);

        void checkStatement(const StatementNode& statement);
        void checkBlockStatement(const BlockStatementNode& blockStmt);
        void checkBreakStatement(const BreakStatementNode& breakStmt);
        void checkContinueStatement(const ContinueStatementNode& continueStmt);
        void checkExpressionStatement(const ExpressionStatementNode& exprStmt);
        void checkForStatement(const ForStatementNode& forStmt);
        void checkIfStatement(const IfStatementNode& ifStmt);
        void checkReloadStatement(const ReloadStatementNode& reloadStmt);
        void checkReturnStatement(const ReturnStatementNode& returnStmt);
        void checkSwitchStatement(const SwitchStatementNode& switchStmt);
        void checkVariableDeclarationStatement(const VariableDeclarationStatementNode& varDeclStmt);
        void checkWhileStatement(const WhileStatementNode& whileStmt);

        void checkExpression(const ExpressionNode& expression);
        void checkBinaryExpression(const BinaryExpressionNode& binaryExpr);
        void checkConditionalExpression(const ConditionalExpressionNode& conditionalExpr);
        void checkDictLiteralExpression(const DictLiteralExpressionNode& dictLiteralExpr);
        void checkFunctionCallExpression(const FunctionCallExpressionNode& funcCallExpr);
        void checkIdentifierLikeExpression(const IdentifierLikeExpressionNode& identifierLikeExpr);
        void checkPrimitiveTypeExpression(const PrimitiveTypeExpressionNode& primitiveTypeExpr);
        void checkListLikeLiteralExpression(const ListLikeLiteralExpressionNode& listLikeLiteralExpr);
        void checkMemberAccessExpression(const MemberAccessExpressionNode& memberAccessExpr);
        void checkRangeExpression(const RangeExpressionNode& rangeExpr);
        void checkSelectorLiteralExpression(const SelectorLiteralExpressionNode& selectorLiteralExpr);
        void checkSimpleLiteralExpression(const SimpleLiteralExpressionNode& simpleLiteralExpr);
        void checkStringLiteralExpression(const StringLiteralExpressionNode& stringLiteralExpr);
        void checkSubscriptExpression(const SubscriptExpressionNode& subscriptExpr);
        void checkSuperExpression(const SuperExpressionNode& superExpr);
        void checkNoneExpression(const NoneExpressionNode& noneExpr);
        void checkThisExpression(const ThisExpressionNode& thisExpr);
        void checkUnaryExpression(const UnaryExpressionNode& unaryExpr);

    public:
        explicit SemanticAnalyzer(const ModuleNode& module);
        SemanticAnalyzer(const ModuleNode& module, const ImportedRootPackageMap& imports);

        SemanticAnalyzer() = delete;
        SemanticAnalyzer(const SemanticAnalyzer&) = delete;
        SemanticAnalyzer& operator=(const SemanticAnalyzer&) = delete;

        SemanticAnalyzer(SemanticAnalyzer&&) noexcept = default;
        SemanticAnalyzer& operator=(SemanticAnalyzer&&) noexcept = delete;

        [[nodiscard]] SemanticResult analyze(const Config& config);
    };
} // namespace vnlc

#endif // VNLC_SEMANTIC_ANALYZER_HPP
