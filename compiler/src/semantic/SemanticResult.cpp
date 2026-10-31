#include "SemanticResult.hpp"
#include "ast/declaration/OperatorDeclarationNode.hpp"
#include "scope/Scope.hpp"
#include "type/Type.hpp"

namespace vnlc {
    SemanticResult::SemanticResult(
        std::vector<Diagnostic>&& errors,
        std::vector<Diagnostic>&& warnings,
        std::vector<Diagnostic>&& notes,
        std::unordered_map<std::string, std::unique_ptr<CustomizedType>>&& customizedTypes,
        std::unordered_map<const AstNode*, std::unique_ptr<Scope>>&& localScopeMap,
        std::unordered_map<const ImportedItem*, std::unique_ptr<Scope>>&& importedScopeMap,
        std::unordered_map<const TypeReferenceNode*, const Type*>&& typeMap,
        std::unordered_map<const ValueDeclarationNode*, const Type*>&& inferredValueTypeMap,
        std::unordered_map<const FunctionDeclarationNode*, const Type*>&& inferredFunctionReturnTypeMap,
        std::unordered_map<const OperatorDeclarationNode*, const Type*>&& inferredOperatorReturnTypeMap,
        std::unordered_map<const ExpressionNode*, ExpressionAnalysisResult>&& expressionAnalysisResultMap,
        std::vector<std::unique_ptr<InheritanceNode>>&& inheritanceNodes,
        std::unordered_map<const TypeDeclarationNode*, InheritanceNode*>&& localInheritanceMap,
        std::unordered_map<const ImportedIdentifier*, InheritanceNode*>&& importedInheritanceMap
    )
        : errors(std::move(errors)),
          warnings(std::move(warnings)),
          notes(std::move(notes)),
          customizedTypes(std::move(customizedTypes)),
          localScopeMap(std::move(localScopeMap)),
          importedScopeMap(std::move(importedScopeMap)),
          typeMap(std::move(typeMap)),
          inferredValueTypeMap(std::move(inferredValueTypeMap)),
          inferredFunctionReturnTypeMap(std::move(inferredFunctionReturnTypeMap)),
          inferredOperatorReturnTypeMap(std::move(inferredOperatorReturnTypeMap)),
          expressionAnalysisResultMap(std::move(expressionAnalysisResultMap)),
          inheritanceNodes(std::move(inheritanceNodes)),
          localInheritanceMap(std::move(localInheritanceMap)),
          importedInheritanceMap(std::move(importedInheritanceMap)) {}

    bool SemanticResult::hasErrors() const {
        return !errors.empty();
    }

    bool SemanticResult::hasWarnings() const {
        return !warnings.empty();
    }

    bool SemanticResult::hasNotes() const {
        return !notes.empty();
    }

    const std::vector<Diagnostic>& SemanticResult::getErrors() const {
        return errors;
    }

    const std::vector<Diagnostic>& SemanticResult::getWarnings() const {
        return warnings;
    }

    const std::vector<Diagnostic>& SemanticResult::getNotes() const {
        return notes;
    }

    const CustomizedType* SemanticResult::getCustomizedTypeByFullTypeName(const std::string& fullTypeName) const {
        auto it = customizedTypes.find(fullTypeName);
        if (it != customizedTypes.end()) {
            return it->second.get();
        }

        return nullptr;
    }

    const Scope* SemanticResult::getScopeByAstNode(const AstNode& node) const {
        auto it = localScopeMap.find(&node);
        if (it != localScopeMap.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const Scope* SemanticResult::getScopeByImportedNode(const ImportedItem& node) const {
        auto it = importedScopeMap.find(&node);
        return it == importedScopeMap.end() ? nullptr : it->second.get();
    }

    const Type* SemanticResult::getTypeByTypeReferenceNode(const TypeReferenceNode* typeNode) const {
        auto it = typeMap.find(typeNode);
        if (it != typeMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    const Type* SemanticResult::getInferredValueType(const ValueDeclarationNode* valueDeclaration) const {
        auto it = inferredValueTypeMap.find(valueDeclaration);
        if (it != inferredValueTypeMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    const Type* SemanticResult::getInferredFunctionReturnType(const FunctionDeclarationNode* functionDeclaration) const {
        auto it = inferredFunctionReturnTypeMap.find(functionDeclaration);
        if (it != inferredFunctionReturnTypeMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    const Type* SemanticResult::getInferredOperatorReturnType(const OperatorDeclarationNode* operatorDeclaration) const {
        auto it = inferredOperatorReturnTypeMap.find(operatorDeclaration);
        if (it != inferredOperatorReturnTypeMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    const ExpressionAnalysisResult* SemanticResult::getExpressionAnalysisResult(const ExpressionNode* expressionNode) const {
        auto it = expressionAnalysisResultMap.find(expressionNode);
        if (it != expressionAnalysisResultMap.end()) {
            return &it->second;
        }
        return nullptr;
    }

    const InheritanceNode* SemanticResult::getInheritanceNodeByLocalNode(const TypeDeclarationNode* localNode) const {
        auto it = localInheritanceMap.find(localNode);
        if (it != localInheritanceMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    const InheritanceNode* SemanticResult::getInheritanceNodeByImportedNode(const ImportedIdentifier* importedNode) const {
        auto it = importedInheritanceMap.find(importedNode);
        if (it != importedInheritanceMap.end()) {
            return it->second;
        }
        return nullptr;
    }
} // namespace vnlc
