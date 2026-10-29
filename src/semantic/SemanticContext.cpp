#include "SemanticContext.hpp"
#include "ast/declaration/OperatorDeclarationNode.hpp"
#include "type/PrimitiveType.hpp"
#include "type/PrimitiveTypeNames.hpp"
#include "vni/import/ImportedPackage.hpp"
#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace vnlc {
    SemanticContext::SemanticContext(const ModuleNode& module, const ImportedRootPackageMap& imports) : module(module), imports(imports) {}

    void SemanticContext::reportError(const AstNode& node, std::string_view message) {
        errors.emplace_back(DiagnosticPhase::SEMANTIC, DiagnosticSeverity::ERROR, std::string(message), node.locate().first, node.locate().second, node.getOffset(), node.getLength());
    }

    void SemanticContext::reportWarning(const AstNode& node, std::string_view message) {
        warnings.emplace_back(DiagnosticPhase::SEMANTIC, DiagnosticSeverity::WARNING, std::string(message), node.locate().first, node.locate().second, node.getOffset(), node.getLength());
    }

    void SemanticContext::reportNote(const AstNode& node, std::string_view message) {
        notes.emplace_back(DiagnosticPhase::SEMANTIC, DiagnosticSeverity::NOTE, std::string(message), node.locate().first, node.locate().second, node.getOffset(), node.getLength());
    }

    const ModuleNode& SemanticContext::getModule() const {
        return module;
    }

    const ImportedRootPackageMap& SemanticContext::getImports() const {
        return imports;
    }

    const ImportedItem* SemanticContext::getImportedItemByFullPath(const std::vector<std::string>& fullPath) const {
        return imports.getImportedItemByFullPath(fullPath);
    }

    const ImportedItem* SemanticContext::getImportedItemByFullTypeName(std::string_view fullTypeName) const {
        return imports.getImportedItemByFullTypeName(fullTypeName);
    }

    const ImportedPackage* SemanticContext::getImportedRootPackageByName(std::string_view name) const {
        return imports.getImportedRootPackageByName(name);
    }

    void SemanticContext::pushScope(std::unique_ptr<Scope>&& scope) {
        scopeStack.push_back(std::move(scope));
    }

    bool SemanticContext::pushExistingScope(const AstNode* astNode) {
        auto it = localScopeMap.find(astNode);
        if (it == localScopeMap.end()) {
            return false;
        }

        scopeStack.push_back(std::move(it->second));
        localScopeMap.erase(it);
        return true;
    }

    void SemanticContext::popScope() {
        if (scopeStack.back()->getOrigin() == ScopeOrigin::IMPORTED) {
            importedScopeMap.emplace(scopeStack.back()->getImportedNode(), std::move(scopeStack.back()));
        } else {
            localScopeMap.emplace(scopeStack.back()->getLocalNode(), std::move(scopeStack.back()));
        }
        scopeStack.pop_back();
    }

    Scope& SemanticContext::getOrCreateImportedScope(ScopeKind kind, const Scope* parent, const ImportedItem& importedNode) {
        for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
            if ((*it)->getOrigin() == ScopeOrigin::IMPORTED && (*it)->getImportedNode() == &importedNode) {
                return **it;
            }
        }
        auto [it, inserted] = importedScopeMap.try_emplace(&importedNode);
        if (inserted) {
            it->second = std::make_unique<Scope>(kind, parent, &importedNode);
        }
        return *it->second;
    }

    void SemanticContext::registerCustomizedType(std::unique_ptr<CustomizedType>&& customizedType) {
        customizedTypeRegistry.emplace(customizedType->getFullTypeName(), std::move(customizedType));
    }

    void SemanticContext::mapType(const TypeReferenceNode* typeNode, const Type* type) {
        typeMap.emplace(typeNode, type);
    }

    void SemanticContext::mapInferredValueType(const ValueDeclarationNode* valueDeclaration, const Type* type) {
        inferredValueTypeMap.emplace(valueDeclaration, type);
    }

    void SemanticContext::mapInferredFunctionReturnType(const FunctionDeclarationNode* functionDeclaration, const Type* type) {
        inferredFunctionReturnTypeMap.emplace(functionDeclaration, type);
    }

    void SemanticContext::mapInferredOperatorReturnType(const OperatorDeclarationNode* operatorDeclaration, const Type* type) {
        inferredOperatorReturnTypeMap.emplace(operatorDeclaration, type);
    }

    void SemanticContext::mapExpressionAnalysisResult(const ExpressionNode* expressionNode, ExpressionAnalysisResult result) {
        expressionAnalysisResultMap.emplace(expressionNode, result);
    }

    ClassInheritanceNode& SemanticContext::getOrCreateLocalInheritanceNode(const ClassDeclarationNode& localNode) {
        auto [it, inserted] = localInheritanceMap.try_emplace(&localNode);
        if (inserted) {
            it->second = inheritanceNodes.emplace_back(std::make_unique<ClassInheritanceNode>(&localNode)).get();
        }
        return static_cast<ClassInheritanceNode&>(*it->second);
    }

    InterfaceInheritanceNode& SemanticContext::getOrCreateLocalInheritanceNode(const InterfaceDeclarationNode& localNode) {
        auto [it, inserted] = localInheritanceMap.try_emplace(&localNode);
        if (inserted) {
            it->second = inheritanceNodes.emplace_back(std::make_unique<InterfaceInheritanceNode>(&localNode)).get();
        }
        return static_cast<InterfaceInheritanceNode&>(*it->second);
    }

    EnumInheritanceNode& SemanticContext::getOrCreateLocalInheritanceNode(const EnumDeclarationNode& localNode) {
        auto [it, inserted] = localInheritanceMap.try_emplace(&localNode);
        if (inserted) {
            it->second = inheritanceNodes.emplace_back(std::make_unique<EnumInheritanceNode>(&localNode)).get();
        }
        return static_cast<EnumInheritanceNode&>(*it->second);
    }

    EnumMemberInheritanceNode& SemanticContext::getOrCreateLocalInheritanceNode(const EnumMemberDeclarationNode& localNode) {
        auto [it, inserted] = localInheritanceMap.try_emplace(&localNode);
        if (inserted) {
            it->second = inheritanceNodes.emplace_back(std::make_unique<EnumMemberInheritanceNode>(&localNode)).get();
        }
        return static_cast<EnumMemberInheritanceNode&>(*it->second);
    }

    ClassInheritanceNode& SemanticContext::getOrCreateImportedInheritanceNode(const ImportedClass& importedNode) {
        auto [it, inserted] = importedInheritanceMap.try_emplace(&importedNode);
        if (inserted) {
            it->second = inheritanceNodes.emplace_back(std::make_unique<ClassInheritanceNode>(&importedNode)).get();
        }
        return static_cast<ClassInheritanceNode&>(*it->second);
    }

    InterfaceInheritanceNode& SemanticContext::getOrCreateImportedInheritanceNode(const ImportedInterface& importedNode) {
        auto [it, inserted] = importedInheritanceMap.try_emplace(&importedNode);
        if (inserted) {
            it->second = inheritanceNodes.emplace_back(std::make_unique<InterfaceInheritanceNode>(&importedNode)).get();
        }
        return static_cast<InterfaceInheritanceNode&>(*it->second);
    }

    EnumInheritanceNode& SemanticContext::getOrCreateImportedInheritanceNode(const ImportedEnum& importedNode) {
        auto [it, inserted] = importedInheritanceMap.try_emplace(&importedNode);
        if (inserted) {
            it->second = inheritanceNodes.emplace_back(std::make_unique<EnumInheritanceNode>(&importedNode)).get();
        }
        return static_cast<EnumInheritanceNode&>(*it->second);
    }

    EnumMemberInheritanceNode& SemanticContext::getOrCreateImportedInheritanceNode(const ImportedEnumMember& importedNode) {
        auto [it, inserted] = importedInheritanceMap.try_emplace(&importedNode);
        if (inserted) {
            it->second = inheritanceNodes.emplace_back(std::make_unique<EnumMemberInheritanceNode>(&importedNode)).get();
        }
        return static_cast<EnumMemberInheritanceNode&>(*it->second);
    }

    const CustomizedType* SemanticContext::getCustomizedTypeByFullTypeName(std::string_view fullTypeName) const {
        auto it = customizedTypeRegistry.find(std::string(fullTypeName));
        if (it != customizedTypeRegistry.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const Type* SemanticContext::getTypeByFullTypeName(std::string_view fullTypeName) const {
        static const std::array<std::string_view, 8> primitiveTypes = {
            PrimitiveTypeNames::Byte,  PrimitiveTypeNames::Short,  PrimitiveTypeNames::Int,     PrimitiveTypeNames::Long,
            PrimitiveTypeNames::Float, PrimitiveTypeNames::Double, PrimitiveTypeNames::Boolean, PrimitiveTypeNames::String,
        };
        const auto getPrimitiveTypeByFullTypeName = [](std::string_view name) -> const PrimitiveType* {
            if (name == PrimitiveTypeNames::Byte) return PrimitiveType::byteType();
            if (name == PrimitiveTypeNames::Short) return PrimitiveType::shortType();
            if (name == PrimitiveTypeNames::Int) return PrimitiveType::intType();
            if (name == PrimitiveTypeNames::Long) return PrimitiveType::longType();
            if (name == PrimitiveTypeNames::Float) return PrimitiveType::floatType();
            if (name == PrimitiveTypeNames::Double) return PrimitiveType::doubleType();
            if (name == PrimitiveTypeNames::Boolean) return PrimitiveType::booleanType();
            if (name == PrimitiveTypeNames::String) return PrimitiveType::stringType();
            return nullptr;
        };

        if (std::find(primitiveTypes.begin(), primitiveTypes.end(), fullTypeName) != primitiveTypes.end()) {
            return getPrimitiveTypeByFullTypeName(fullTypeName);
        } else {
            return getCustomizedTypeByFullTypeName(fullTypeName);
        }
    }

    const Type* SemanticContext::getTypeByTypeReferenceNode(const TypeReferenceNode* typeNode) const {
        auto it = typeMap.find(typeNode);
        if (it != typeMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    const Scope* SemanticContext::getScopeByAstNode(const AstNode* astNode) const {
        if (astNode == nullptr) return nullptr;
        for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
            if ((*it)->getOrigin() == ScopeOrigin::LOCAL && (*it)->getLocalNode() == astNode) {
                return it->get();
            }
        }

        auto it = localScopeMap.find(astNode);
        if (it != localScopeMap.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const Scope* SemanticContext::getScopeByImportedNode(const ImportedItem* importedNode) const {
        if (importedNode == nullptr) return nullptr;
        for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
            if ((*it)->getOrigin() == ScopeOrigin::IMPORTED && (*it)->getImportedNode() == importedNode) {
                return it->get();
            }
        }
        auto it = importedScopeMap.find(importedNode);
        return it == importedScopeMap.end() ? nullptr : it->second.get();
    }

    const Scope* SemanticContext::getScopeByRegularSymbol(const RegularSymbol& symbol) const {
        if (symbol.getLocalNode() != nullptr) {
            return getScopeByAstNode(symbol.getLocalNode());
        }
        if (symbol.getImportedNode() != nullptr) {
            return getScopeByImportedNode(symbol.getImportedNode());
        }
        return nullptr;
    }

    const InheritanceNode* SemanticContext::getInheritanceNodeByLocalNode(const TypeDeclarationNode* localNode) const {
        auto it = localInheritanceMap.find(localNode);
        if (it != localInheritanceMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    const InheritanceNode* SemanticContext::getInheritanceNodeByImportedNode(const ImportedIdentifier* importedNode) const {
        auto it = importedInheritanceMap.find(importedNode);
        if (it != importedInheritanceMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    const ExpressionAnalysisResult* SemanticContext::getExpressionAnalysisResult(const ExpressionNode* expressionNode) const {
        auto it = expressionAnalysisResultMap.find(expressionNode);
        if (it != expressionAnalysisResultMap.end()) {
            return &it->second;
        }
        return nullptr;
    }

    Scope& SemanticContext::currentScope() {
        return *scopeStack.back();
    }

    const Scope* SemanticContext::currentModuleScope() const noexcept {
        const Scope* current = scopeStack.back().get();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::MODULE) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const Scope* SemanticContext::currentFunctionScope() const noexcept {
        const Scope* current = scopeStack.back().get();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::FUNCTION) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const Scope* SemanticContext::currentClassScope() const noexcept {
        const Scope* current = scopeStack.back().get();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::CLASS) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const Scope* SemanticContext::currentInterfaceScope() const noexcept {
        const Scope* current = scopeStack.back().get();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::INTERFACE) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const Scope* SemanticContext::currentEnumScope() const noexcept {
        const Scope* current = scopeStack.back().get();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::ENUM) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const Scope* SemanticContext::currentEnumMemberScope() const noexcept {
        const Scope* current = scopeStack.back().get();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::ENUM_MEMBER) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const Scope* SemanticContext::currentBlockScope() const noexcept {
        const Scope* current = scopeStack.back().get();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::BLOCK) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const Scope* SemanticContext::currentLoopScope() const noexcept {
        const Scope* current = scopeStack.back().get();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::LOOP) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const Scope* SemanticContext::currentSwitchScope() const noexcept {
        const Scope* current = scopeStack.back().get();
        while (current != nullptr) {
            if (current->getKind() == ScopeKind::SWITCH) {
                return current;
            }
            current = current->findParent();
        }

        return nullptr;
    }

    const std::vector<Diagnostic>& SemanticContext::getErrors() const noexcept {
        return errors;
    }

    const std::vector<Diagnostic>& SemanticContext::getWarnings() const noexcept {
        return warnings;
    }

    const std::vector<Diagnostic>& SemanticContext::getNotes() const noexcept {
        return notes;
    }

    std::tuple<std::vector<Diagnostic>, std::vector<Diagnostic>, std::vector<Diagnostic>> SemanticContext::takeDiagnostics() {
        return std::make_tuple(std::move(errors), std::move(warnings), std::move(notes));
    }

    std::unordered_map<const AstNode*, std::unique_ptr<Scope>> SemanticContext::takeLocalScopeMap() {
        return std::move(localScopeMap);
    }

    std::unordered_map<const ImportedItem*, std::unique_ptr<Scope>> SemanticContext::takeImportedScopeMap() {
        return std::move(importedScopeMap);
    }

    std::unordered_map<std::string, std::unique_ptr<CustomizedType>> SemanticContext::takeCustomizedTypeRegistry() {
        return std::move(customizedTypeRegistry);
    }

    std::unordered_map<const TypeReferenceNode*, const Type*> SemanticContext::takeTypeMap() {
        return std::move(typeMap);
    }

    std::unordered_map<const ValueDeclarationNode*, const Type*> SemanticContext::takeInferredValueTypeMap() {
        return std::move(inferredValueTypeMap);
    }

    std::unordered_map<const FunctionDeclarationNode*, const Type*> SemanticContext::takeInferredFunctionReturnTypeMap() {
        return std::move(inferredFunctionReturnTypeMap);
    }

    std::unordered_map<const OperatorDeclarationNode*, const Type*> SemanticContext::takeInferredOperatorReturnTypeMap() {
        return std::move(inferredOperatorReturnTypeMap);
    }

    std::unordered_map<const ExpressionNode*, ExpressionAnalysisResult> SemanticContext::takeExpressionAnalysisResultMap() {
        return std::move(expressionAnalysisResultMap);
    }

    std::vector<std::unique_ptr<InheritanceNode>> SemanticContext::takeInheritanceNodes() {
        return std::move(inheritanceNodes);
    }

    std::unordered_map<const TypeDeclarationNode*, InheritanceNode*> SemanticContext::takeLocalInheritanceMap() {
        return std::move(localInheritanceMap);
    }

    std::unordered_map<const ImportedIdentifier*, InheritanceNode*> SemanticContext::takeImportedInheritanceMap() {
        return std::move(importedInheritanceMap);
    }
} // namespace vnlc
