#include "SemanticAnalyzer.hpp"
#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/declaration/ConstructorDeclarationNode.hpp"
#include "ast/declaration/DeclarationNode.hpp"
#include "ast/declaration/EnumDeclarationNode.hpp"
#include "ast/declaration/EnumMemberDeclarationNode.hpp"
#include "ast/declaration/FunctionDeclarationKind.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include "ast/declaration/OperatorDeclarationNode.hpp"
#include "ast/declaration/TypeAliasDeclarationNode.hpp"
#include "ast/declaration/TypeDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationKind.hpp"
#include "ast/expression/BinaryExpressionNode.hpp"
#include "ast/expression/ConditionalExpressionNode.hpp"
#include "ast/expression/DictLiteralExpressionNode.hpp"
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
#include "ast/typeref/CustomizedTypeReferenceNode.hpp"
#include "ast/typeref/PrimitiveTypeReferenceNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include "fmt/format.h"
#include "inheritance/ClassInheritanceNode.hpp"
#include "semantic/ExpressionAnalysisResult.hpp"
#include "symbol/FunctionSymbol.hpp"
#include "symbol/RegularSymbol.hpp"
#include "symbol/RegularSymbolAccessModifier.hpp"
#include "symbol/RegularSymbolKind.hpp"
#include "symbol/RegularSymbolOrigin.hpp"
#include "type/CustomizedType.hpp"
#include "type/CustomizedTypeKind.hpp"
#include "type/CustomizedTypeOrigin.hpp"
#include "type/PrimitiveType.hpp"
#include "type/PrimitiveTypeNames.hpp"
#include "type/StandardLibraryTypeNames.hpp"
#include "type/Type.hpp"
#include "vni/import/ImportedAlias.hpp"
#include "vni/import/ImportedClass.hpp"
#include "vni/import/ImportedConstructor.hpp"
#include "vni/import/ImportedEnum.hpp"
#include "vni/import/ImportedEnumMember.hpp"
#include "vni/import/ImportedEnumValue.hpp"
#include "vni/import/ImportedFunc.hpp"
#include "vni/import/ImportedIdentifier.hpp"
#include "vni/import/ImportedInterface.hpp"
#include "vni/import/ImportedItem.hpp"
#include "vni/import/ImportedLet.hpp"
#include "vni/import/ImportedMethod.hpp"
#include "vni/import/ImportedModule.hpp"
#include "vni/import/ImportedOperator.hpp"
#include "vni/import/ImportedPackage.hpp"
#include "vni/import/ImportedParameter.hpp"
#include "vni/import/ImportedProperty.hpp"
#include "vni/import/ImportedTypeAlias.hpp"
#include <algorithm>
#include <fmt/core.h>
#include <functional>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_set>

namespace vnlc {
    namespace {
        RegularSymbolKind getImportedSymbolKind(const ImportedItem& item) {
            if (dynamic_cast<const ImportedPackage*>(&item)) return RegularSymbolKind::PACKAGE;
            if (dynamic_cast<const ImportedModule*>(&item)) return RegularSymbolKind::MODULE;
            if (dynamic_cast<const ImportedLet*>(&item)) return RegularSymbolKind::VARIABLE;
            if (dynamic_cast<const ImportedFunc*>(&item)) return RegularSymbolKind::FUNCTION;
            if (dynamic_cast<const ImportedClass*>(&item)) return RegularSymbolKind::CLASS;
            if (dynamic_cast<const ImportedInterface*>(&item)) return RegularSymbolKind::INTERFACE;
            if (dynamic_cast<const ImportedEnum*>(&item)) return RegularSymbolKind::ENUM;
            if (dynamic_cast<const ImportedTypeAlias*>(&item)) return RegularSymbolKind::TYPE_ALIAS;
            if (dynamic_cast<const ImportedEnumMember*>(&item)) return RegularSymbolKind::ENUM_MEMBER;
            if (dynamic_cast<const ImportedProperty*>(&item) || dynamic_cast<const ImportedEnumValue*>(&item)) return RegularSymbolKind::PROPERTY;
            if (dynamic_cast<const ImportedMethod*>(&item)) return RegularSymbolKind::METHOD;
            if (dynamic_cast<const ImportedConstructor*>(&item)) return RegularSymbolKind::METHOD;
            if (dynamic_cast<const ImportedParameter*>(&item)) return RegularSymbolKind::PARAMETER;
            return RegularSymbolKind::IMPORT_ALIAS;
        }

        std::optional<ScopeKind> getImportedScopeKind(const ImportedItem& item) {
            switch (getImportedSymbolKind(item)) {
                case RegularSymbolKind::PACKAGE:
                    return ScopeKind::PACKAGE;
                case RegularSymbolKind::MODULE:
                    return ScopeKind::MODULE;
                case RegularSymbolKind::CLASS:
                    return ScopeKind::CLASS;
                case RegularSymbolKind::INTERFACE:
                    return ScopeKind::INTERFACE;
                case RegularSymbolKind::ENUM:
                    return ScopeKind::ENUM;
                case RegularSymbolKind::ENUM_MEMBER:
                    return ScopeKind::ENUM_MEMBER;
                case RegularSymbolKind::TYPE_ALIAS:
                    return ScopeKind::TYPE_ALIAS;
                case RegularSymbolKind::FUNCTION:
                case RegularSymbolKind::METHOD:
                    return ScopeKind::FUNCTION;
                default:
                    return std::nullopt;
            }
        }

        RegularSymbolAccessModifier getImportedAccessModifier(const ImportedItem& item) {
            std::string_view accessModifier;
            if (const auto* property = dynamic_cast<const ImportedProperty*>(&item)) {
                accessModifier = property->getAccessModifier();
            } else if (const auto* method = dynamic_cast<const ImportedMethod*>(&item)) {
                accessModifier = method->getAccessModifier();
            } else if (const auto* constructor = dynamic_cast<const ImportedConstructor*>(&item)) {
                accessModifier = constructor->getAccessModifier();
            }
            if (accessModifier == "private") return RegularSymbolAccessModifier::PRIVATE;
            if (accessModifier == "protected") return RegularSymbolAccessModifier::PROTECTED;
            return RegularSymbolAccessModifier::PUBLIC;
        }
    } // namespace

    SemanticAnalyzer::SemanticAnalyzer(const ModuleNode& module, const ImportedRootPackageMap& imports) : context(module, imports) {}

    void SemanticAnalyzer::declareFunctionOverloading(const FunctionDeclarationNode& funcDecl, RegularSymbolKind kind, std::string_view declarationKind) {
        const auto name = funcDecl.getName().getIdentifierString();
        const auto internalName = funcDecl.getInternalName();
        const Symbol* existingSymbol = context.currentScope().lookupLocal(name);

        if (existingSymbol != nullptr) {
            const auto* existingFunction = dynamic_cast<const FunctionSymbol*>(existingSymbol);
            if (existingFunction == nullptr) {
                context.reportError(funcDecl, fmt::format("Redeclaration of {} '{}'", declarationKind, name));
                return;
            }
            if (existingFunction->getOverloadingByInternalName(internalName) != nullptr) {
                context.reportError(funcDecl, fmt::format("Redeclaration of {} '{}'", declarationKind, internalName));
                return;
            }
        }

        FunctionSymbol functionSymbol(name);
        functionSymbol.addOverloading(RegularSymbol(kind, static_cast<RegularSymbolAccessModifier>(funcDecl.getAccessModifier()), internalName, &funcDecl));

        if (!context.currentScope().declare(std::move(functionSymbol))) {
            context.reportError(funcDecl, fmt::format("Redeclaration of {} '{}'", declarationKind, name));
        }
    }

    void SemanticAnalyzer::registerImportedTypeReference(std::string_view fullTypeName) {
        const auto isSpace = [](char character) { return character == ' ' || character == '\t' || character == '\r' || character == '\n'; };
        const auto trim = [&isSpace](std::string_view value) {
            while (!value.empty() && isSpace(value.front())) {
                value.remove_prefix(1);
            }
            while (!value.empty() && isSpace(value.back())) {
                value.remove_suffix(1);
            }
            return value;
        };
        fullTypeName = trim(fullTypeName);

        if (fullTypeName.empty()) {
            return;
        }
        if (context.getTypeByFullTypeName(fullTypeName)) {
            return;
        }

        const std::size_t genericArgumentStart = fullTypeName.find('<');

        std::vector<const Type*> genericArgumentTypes;
        if (genericArgumentStart != std::string_view::npos) {
            std::size_t depth = 0;
            std::size_t genericArgumentEnd = std::string_view::npos;
            for (std::size_t index = genericArgumentStart; index < fullTypeName.size(); ++index) {
                if (fullTypeName[index] == '<') {
                    ++depth;
                } else if (fullTypeName[index] == '>') {
                    --depth;
                    if (depth == 0) {
                        genericArgumentEnd = index;
                        break;
                    }
                }
            }

            if (genericArgumentEnd == std::string_view::npos) {
                return;
            }

            std::size_t argumentStart = genericArgumentStart + 1;
            depth = 0;
            for (std::size_t index = argumentStart; index < genericArgumentEnd; ++index) {
                if (fullTypeName[index] == '<') {
                    ++depth;
                } else if (fullTypeName[index] == '>') {
                    --depth;
                } else if (fullTypeName[index] == ',' && depth == 0) {
                    const std::string_view argument = trim(fullTypeName.substr(argumentStart, index - argumentStart));
                    registerImportedTypeReference(argument);
                    const Type* argumentType = context.getTypeByFullTypeName(argument);
                    if (argumentType == nullptr) {
                        return;
                    }
                    genericArgumentTypes.emplace_back(argumentType);
                    argumentStart = index + 1;
                }
            }

            const std::string_view lastArgument = trim(fullTypeName.substr(argumentStart, genericArgumentEnd - argumentStart));
            registerImportedTypeReference(lastArgument);
            const Type* lastArgumentType = context.getTypeByFullTypeName(lastArgument);
            if (lastArgumentType == nullptr) {
                return;
            }
            genericArgumentTypes.emplace_back(lastArgumentType);
        }

        const ImportedItem* importedType = context.getImportedItemByFullTypeName(fullTypeName);

        // imported type references should never be type aliases
        const auto customizedTypeKind = [&importedType]() -> std::optional<CustomizedTypeKind> {
            if (dynamic_cast<const ImportedClass*>(importedType) != nullptr) return CustomizedTypeKind::CLASS;
            if (dynamic_cast<const ImportedInterface*>(importedType) != nullptr) return CustomizedTypeKind::INTERFACE;
            if (dynamic_cast<const ImportedEnum*>(importedType) != nullptr) return CustomizedTypeKind::ENUM;
            if (dynamic_cast<const ImportedEnumMember*>(importedType) != nullptr) return CustomizedTypeKind::ENUM_MEMBER;
            return std::nullopt;
        }();
        if (!customizedTypeKind.has_value()) {
            return;
        }

        const auto* importedIdentifier = dynamic_cast<const ImportedIdentifier*>(importedType);
        if (importedIdentifier == nullptr) {
            return;
        }

        auto customizedType = std::make_unique<CustomizedType>(customizedTypeKind.value(), fullTypeName, std::move(genericArgumentTypes), importedIdentifier);
        context.registerCustomizedType(std::move(customizedType));
        return;
    }

    void SemanticAnalyzer::registerImportedTypeReferences(const ImportedItem& importedItem) {
        const auto registerChildren = [&](const auto& children) {
            for (const auto& [name, child] : children) {
                registerImportedTypeReferences(*child);
            }
        };
        const auto registerParameters = [&](const auto& parameters) {
            for (const auto& [name, parameter] : parameters) {
                registerImportedTypeReference(parameter->getType());
            }
        };

        if (const auto* let = dynamic_cast<const ImportedLet*>(&importedItem)) {
            registerImportedTypeReference(let->getType());
        } else if (const auto* function = dynamic_cast<const ImportedFunc*>(&importedItem)) {
            registerImportedTypeReference(function->getReturnType());
            registerParameters(function->getParameters());
        } else if (const auto* method = dynamic_cast<const ImportedMethod*>(&importedItem)) {
            registerImportedTypeReference(method->getReturnType());
            registerParameters(method->getParameters());
        } else if (const auto* operatorNode = dynamic_cast<const ImportedOperator*>(&importedItem)) {
            registerImportedTypeReference(operatorNode->getReturnType());
            registerParameters(operatorNode->getParameters());
        } else if (const auto* constructor = dynamic_cast<const ImportedConstructor*>(&importedItem)) {
            registerParameters(constructor->getParameters());
        } else if (const auto* property = dynamic_cast<const ImportedProperty*>(&importedItem)) {
            registerImportedTypeReference(property->getType());
        } else if (const auto* parameter = dynamic_cast<const ImportedParameter*>(&importedItem)) {
            registerImportedTypeReference(parameter->getType());
        } else if (const auto* classType = dynamic_cast<const ImportedClass*>(&importedItem)) {
            if (classType->getBaseClass().has_value()) {
                registerImportedTypeReference(classType->getBaseClass().value());
            }
            for (const auto& interfaceName : classType->getImplementedInterfaces()) {
                registerImportedTypeReference(interfaceName);
            }
            registerChildren(classType->getProperties());
            registerChildren(classType->getMethods());
            registerChildren(classType->getConstructors());
            registerChildren(classType->getOperators());
        } else if (const auto* interfaceType = dynamic_cast<const ImportedInterface*>(&importedItem)) {
            registerChildren(interfaceType->getMethods());
            registerChildren(interfaceType->getOperators());
        } else if (const auto* enumType = dynamic_cast<const ImportedEnum*>(&importedItem)) {
            registerChildren(enumType->getMembers());
        } else if (const auto* enumMember = dynamic_cast<const ImportedEnumMember*>(&importedItem)) {
            registerChildren(enumMember->getAssociatedValues());
        } else if (const auto* enumValue = dynamic_cast<const ImportedEnumValue*>(&importedItem)) {
            registerImportedTypeReference(enumValue->getType());
        } else if (const auto* typeAlias = dynamic_cast<const ImportedTypeAlias*>(&importedItem)) {
            registerImportedTypeReference(typeAlias->getOriginalType());
        } else if (const auto* module = dynamic_cast<const ImportedModule*>(&importedItem)) {
            registerChildren(module->getIdentifiers());
        } else if (const auto* package = dynamic_cast<const ImportedPackage*>(&importedItem)) {
            registerChildren(package->getSubPackages());
            registerChildren(package->getModules());
        }
    }

    void SemanticAnalyzer::registerImportedScopes(const ImportedItem& item, const Scope* parent) {
        const auto kind = getImportedScopeKind(item);
        if (!kind.has_value()) return;
        auto& scope = context.getOrCreateImportedScope(kind.value(), parent, item);
        const auto declareChildren = [&](const auto& children) {
            for (const auto& [name, child] : children) {
                registerImportedScopes(*child, &scope);

                if (dynamic_cast<const ImportedOperator*>(child.get()) != nullptr) continue;
                if (dynamic_cast<const ImportedConstructor*>(child.get()) != nullptr) continue;
                if (const auto* function = dynamic_cast<const ImportedFunc*>(child.get())) {
                    FunctionSymbol functionSymbol(function->getOriginalName());
                    functionSymbol.addOverloading(RegularSymbol(RegularSymbolKind::FUNCTION, getImportedAccessModifier(*child), child->getName(), child.get()));
                    scope.declare(std::move(functionSymbol));
                } else if (const auto* method = dynamic_cast<const ImportedMethod*>(child.get())) {
                    FunctionSymbol functionSymbol(method->getOriginalName());
                    functionSymbol.addOverloading(RegularSymbol(RegularSymbolKind::METHOD, getImportedAccessModifier(*child), child->getName(), child.get()));
                    scope.declare(std::move(functionSymbol));
                } else {
                    scope.declare(RegularSymbol(getImportedSymbolKind(*child), getImportedAccessModifier(*child), name, child.get()));
                }
            }
        };
        const auto declareGenericParameters = [&](const auto& parameters) {
            for (const auto& name : parameters) {
                scope.declare(RegularSymbol(RegularSymbolKind::GENERIC_PARAMETER, RegularSymbolAccessModifier::PUBLIC, name, &item));
            }
        };

        if (const auto* package = dynamic_cast<const ImportedPackage*>(&item)) {
            declareChildren(package->getSubPackages());
            declareChildren(package->getModules());
        } else if (const auto* module = dynamic_cast<const ImportedModule*>(&item)) {
            declareChildren(module->getIdentifiers());
        } else if (const auto* classType = dynamic_cast<const ImportedClass*>(&item)) {
            declareChildren(classType->getProperties());
            declareChildren(classType->getMethods());
            declareChildren(classType->getConstructors());
            declareChildren(classType->getOperators());
            declareGenericParameters(classType->getGenericParameters());
        } else if (const auto* interfaceType = dynamic_cast<const ImportedInterface*>(&item)) {
            declareChildren(interfaceType->getMethods());
            declareChildren(interfaceType->getOperators());
            declareGenericParameters(interfaceType->getGenericParameters());
        } else if (const auto* enumType = dynamic_cast<const ImportedEnum*>(&item)) {
            declareChildren(enumType->getMembers());
            declareGenericParameters(enumType->getGenericParameters());
        } else if (const auto* enumMember = dynamic_cast<const ImportedEnumMember*>(&item)) {
            declareChildren(enumMember->getAssociatedValues());
        } else if (const auto* typeAlias = dynamic_cast<const ImportedTypeAlias*>(&item)) {
            declareGenericParameters(typeAlias->getGenericParameters());
        } else if (const auto* function = dynamic_cast<const ImportedFunc*>(&item)) {
            declareChildren(function->getParameters());
        } else if (const auto* method = dynamic_cast<const ImportedMethod*>(&item)) {
            declareChildren(method->getParameters());
        } else if (const auto* constructor = dynamic_cast<const ImportedConstructor*>(&item)) {
            declareChildren(constructor->getParameters());
        }
    }

    void SemanticAnalyzer::collectLocalRegularSymbolsInModule(const ModuleNode& moduleNode) {
        for (const auto& topIdentifierDecl : moduleNode.getTopIdentifierDeclarations()) {
            DeclarationNode* declNode = topIdentifierDecl.get();

            if (auto* varDecl = dynamic_cast<ValueDeclarationNode*>(declNode)) {
                RegularSymbol symbol(RegularSymbolKind::VARIABLE, static_cast<RegularSymbolAccessModifier>(varDecl->getAccessModifier()), varDecl->getName().getIdentifierString(), varDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*varDecl, fmt::format("Redeclaration of symbol '{}'", varDecl->getName().getIdentifierString()));
                }
            } else if (auto* classDecl = dynamic_cast<ClassDeclarationNode*>(declNode)) {
                RegularSymbol symbol(RegularSymbolKind::CLASS, RegularSymbolAccessModifier::PUBLIC, classDecl->getName().getIdentifierString(), classDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*classDecl, fmt::format("Redeclaration of symbol '{}'", classDecl->getName().getIdentifierString()));
                }
            } else if (auto* interfaceDecl = dynamic_cast<InterfaceDeclarationNode*>(declNode)) {
                RegularSymbol symbol(RegularSymbolKind::INTERFACE, RegularSymbolAccessModifier::PUBLIC, interfaceDecl->getName().getIdentifierString(), interfaceDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*interfaceDecl, fmt::format("Redeclaration of symbol '{}'", interfaceDecl->getName().getIdentifierString()));
                }
            } else if (auto* enumDecl = dynamic_cast<EnumDeclarationNode*>(declNode)) {
                RegularSymbol symbol(RegularSymbolKind::ENUM, RegularSymbolAccessModifier::PUBLIC, enumDecl->getName().getIdentifierString(), enumDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*enumDecl, fmt::format("Redeclaration of symbol '{}'", enumDecl->getName().getIdentifierString()));
                }
            } else if (auto* typeAliasDecl = dynamic_cast<TypeAliasDeclarationNode*>(declNode)) {
                RegularSymbol symbol(RegularSymbolKind::TYPE_ALIAS, RegularSymbolAccessModifier::PUBLIC, typeAliasDecl->getAliasName().getIdentifierString(), typeAliasDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*typeAliasDecl, fmt::format("Redeclaration of symbol '{}'", typeAliasDecl->getAliasName().getIdentifierString()));
                }
            }
        }

        for (const auto& topIdentifierDecl : moduleNode.getTopIdentifierDeclarations()) {
            DeclarationNode* declNode = topIdentifierDecl.get();

            if (auto* funcDecl = dynamic_cast<FunctionDeclarationNode*>(declNode)) {
                collectLocalRegularSymbolsInFunction(*funcDecl);
            } else if (auto* classDecl = dynamic_cast<ClassDeclarationNode*>(declNode)) {
                collectLocalRegularSymbolsInClass(*classDecl);
            } else if (auto* interfaceDecl = dynamic_cast<InterfaceDeclarationNode*>(declNode)) {
                collectLocalRegularSymbolsInInterface(*interfaceDecl);
            } else if (auto* enumDecl = dynamic_cast<EnumDeclarationNode*>(declNode)) {
                collectLocalRegularSymbolsInEnum(*enumDecl);
            } else if (auto* typeAliasDecl = dynamic_cast<TypeAliasDeclarationNode*>(declNode)) {
                collectLocalRegularSymbolsInTypeAlias(*typeAliasDecl);
            }
        }
    }

    void SemanticAnalyzer::collectLocalRegularSymbolsInFunction(const FunctionDeclarationNode& functionNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::FUNCTION, &context.currentScope(), &functionNode));

        for (const auto& parameter : functionNode.getParameters()) {
            RegularSymbol parameterSymbol(RegularSymbolKind::PARAMETER, RegularSymbolAccessModifier::PUBLIC, parameter->getName().getIdentifierString(), parameter.get());
            if (!context.currentScope().declare(std::move(parameterSymbol))) {
                context.reportError(*parameter, fmt::format("Redeclaration of parameter '{}'", parameter->getName().getIdentifierString()));
            }
        }

        context.popScope();
    }

    void SemanticAnalyzer::collectLocalRegularSymbolsInConstructor(const ConstructorDeclarationNode& constructorNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::FUNCTION, &context.currentScope(), &constructorNode));

        for (const auto& parameter : constructorNode.getParameters()) {
            RegularSymbol parameterSymbol(RegularSymbolKind::PARAMETER, RegularSymbolAccessModifier::PUBLIC, parameter->getName().getIdentifierString(), parameter.get());
            if (!context.currentScope().declare(std::move(parameterSymbol))) {
                context.reportError(*parameter, fmt::format("Redeclaration of parameter '{}'", parameter->getName().getIdentifierString()));
            }
        }

        context.popScope();
    }

    void SemanticAnalyzer::collectLocalRegularSymbolsInOperator(const OperatorDeclarationNode& operatorNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::FUNCTION, &context.currentScope(), &operatorNode));

        for (const auto& parameter : operatorNode.getParameters()) {
            RegularSymbol parameterSymbol(RegularSymbolKind::PARAMETER, RegularSymbolAccessModifier::PUBLIC, parameter->getName().getIdentifierString(), parameter.get());
            if (!context.currentScope().declare(std::move(parameterSymbol))) {
                context.reportError(*parameter, fmt::format("Redeclaration of parameter '{}'", parameter->getName().getIdentifierString()));
            }
        }

        context.popScope();
    }

    void SemanticAnalyzer::collectLocalRegularSymbolsInClass(const ClassDeclarationNode& classNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::CLASS, &context.currentScope(), &classNode));

        for (const auto& propertyDecl : classNode.getPropertyDeclarations()) {
            RegularSymbol memberSymbol(
                RegularSymbolKind::PROPERTY,
                static_cast<RegularSymbolAccessModifier>(propertyDecl->getAccessModifier()),
                propertyDecl->getName().getIdentifierString(),
                propertyDecl.get()
            );
            if (!context.currentScope().declare(std::move(memberSymbol))) {
                context.reportError(*propertyDecl, fmt::format("Redeclaration of class member '{}'", propertyDecl->getName().getIdentifierString()));
            }
        }

        for (const auto& genericParamName : classNode.getGenericParameterNames()) {
            RegularSymbol genericParamSymbol(RegularSymbolKind::GENERIC_PARAMETER, RegularSymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &classNode);
            if (!context.currentScope().declare(std::move(genericParamSymbol))) {
                context.reportError(classNode, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
            }
        }

        for (const auto& methodDecl : classNode.getMethodDeclarations()) {
            collectLocalRegularSymbolsInFunction(*methodDecl);
        }

        for (const auto& constructorDecl : classNode.getConstructorDeclarations()) {
            collectLocalRegularSymbolsInConstructor(*constructorDecl);
        }

        for (const auto& operatorDecl : classNode.getOperatorDeclarations()) {
            collectLocalRegularSymbolsInOperator(*operatorDecl);
        }

        context.popScope();
    }

    void SemanticAnalyzer::collectLocalRegularSymbolsInInterface(const InterfaceDeclarationNode& interfaceNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::INTERFACE, &context.currentScope(), &interfaceNode));

        for (const auto& genericParamName : interfaceNode.getGenericParameterNames()) {
            RegularSymbol genericParamSymbol(RegularSymbolKind::GENERIC_PARAMETER, RegularSymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &interfaceNode);
            if (!context.currentScope().declare(std::move(genericParamSymbol))) {
                context.reportError(interfaceNode, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
            }
        }

        for (const auto& methodDecl : interfaceNode.getMethodDeclarations()) {
            collectLocalRegularSymbolsInFunction(*methodDecl);
        }

        for (const auto& operatorDecl : interfaceNode.getOperatorDeclarations()) {
            collectLocalRegularSymbolsInOperator(*operatorDecl);
        }

        context.popScope();
    }

    void SemanticAnalyzer::collectLocalRegularSymbolsInEnum(const EnumDeclarationNode& enumNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::ENUM, &context.currentScope(), &enumNode));

        for (const auto& member : enumNode.getMemberDeclarations()) {
            RegularSymbol memberSymbol(RegularSymbolKind::ENUM_MEMBER, RegularSymbolAccessModifier::PUBLIC, member->getName().getIdentifierString(), member.get());
            if (!context.currentScope().declare(std::move(memberSymbol))) {
                context.reportError(*member, fmt::format("Redeclaration of enum member '{}'", member->getName().getIdentifierString()));
            }
        }

        for (const auto& genericParamName : enumNode.getGenericParameterNames()) {
            RegularSymbol genericParamSymbol(RegularSymbolKind::GENERIC_PARAMETER, RegularSymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &enumNode);
            if (!context.currentScope().declare(std::move(genericParamSymbol))) {
                context.reportError(enumNode, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
            }
        }

        for (const auto& member : enumNode.getMemberDeclarations()) {
            collectLocalRegularSymbolsInEnumMember(*member);
        }

        context.popScope();
    }

    void SemanticAnalyzer::collectLocalRegularSymbolsInEnumMember(const EnumMemberDeclarationNode& enumMemberNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::ENUM_MEMBER, &context.currentScope(), &enumMemberNode));

        for (const auto& associatedValue : enumMemberNode.getAssociatedValues()) {
            RegularSymbol associatedValueSymbol(RegularSymbolKind::PROPERTY, RegularSymbolAccessModifier::PUBLIC, associatedValue->getName().getIdentifierString(), associatedValue.get());
            if (!context.currentScope().declare(std::move(associatedValueSymbol))) {
                context.reportError(*associatedValue, fmt::format("Redeclaration of enum member associated value '{}'", associatedValue->getName().getIdentifierString()));
            }
        }

        context.popScope();
    }

    void SemanticAnalyzer::collectLocalRegularSymbolsInTypeAlias(const TypeAliasDeclarationNode& typeAliasNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::TYPE_ALIAS, &context.currentScope(), &typeAliasNode));

        for (const auto& genericParamName : typeAliasNode.getGenericParameterNames()) {
            RegularSymbol genericParamSymbol(RegularSymbolKind::GENERIC_PARAMETER, RegularSymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &typeAliasNode);
            if (!context.currentScope().declare(std::move(genericParamSymbol))) {
                context.reportError(typeAliasNode, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
            }
        }

        context.popScope();
    }

    void SemanticAnalyzer::resolveTypeReference(const TypeReferenceNode& typeRef) {
        using TypeSubstitutionMap = std::unordered_map<std::string, const Type*>;
        using VisitedSet = std::unordered_set<const TypeReferenceNode*>;

        const auto getPrimitiveType = [](PrimitiveTypeReferenceKind kind) -> const PrimitiveType* {
            switch (kind) {
                case PrimitiveTypeReferenceKind::BYTE:
                    return PrimitiveType::byteType();
                case PrimitiveTypeReferenceKind::SHORT:
                    return PrimitiveType::shortType();
                case PrimitiveTypeReferenceKind::INT:
                    return PrimitiveType::intType();
                case PrimitiveTypeReferenceKind::LONG:
                    return PrimitiveType::longType();
                case PrimitiveTypeReferenceKind::FLOAT:
                    return PrimitiveType::floatType();
                case PrimitiveTypeReferenceKind::DOUBLE:
                    return PrimitiveType::doubleType();
                case PrimitiveTypeReferenceKind::BOOLEAN:
                    return PrimitiveType::booleanType();
                case PrimitiveTypeReferenceKind::STRING:
                    return PrimitiveType::stringType();
            }
            return nullptr;
        };

        const auto getGenericParameterCount = [](const RegularSymbol& symbol) -> std::size_t {
            if (symbol.getKind() == RegularSymbolKind::GENERIC_PARAMETER) {
                return 0;
            }

            if (symbol.getOrigin() == RegularSymbolOrigin::LOCAL) {
                if (const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(symbol.getLocalNode())) {
                    return classDeclaration->getGenericParameterNames().size();
                }
                if (const auto* interfaceDeclaration = dynamic_cast<const InterfaceDeclarationNode*>(symbol.getLocalNode())) {
                    return interfaceDeclaration->getGenericParameterNames().size();
                }
                if (const auto* enumDeclaration = dynamic_cast<const EnumDeclarationNode*>(symbol.getLocalNode())) {
                    return enumDeclaration->getGenericParameterNames().size();
                }
                if (const auto* typeAliasDeclaration = dynamic_cast<const TypeAliasDeclarationNode*>(symbol.getLocalNode())) {
                    return typeAliasDeclaration->getGenericParameterNames().size();
                }
            } else if (symbol.getOrigin() == RegularSymbolOrigin::IMPORTED) {
                if (const auto* importedClass = dynamic_cast<const ImportedClass*>(symbol.getImportedNode())) {
                    return importedClass->getGenericParameters().size();
                }
                if (const auto* importedInterface = dynamic_cast<const ImportedInterface*>(symbol.getImportedNode())) {
                    return importedInterface->getGenericParameters().size();
                }
                if (const auto* importedEnum = dynamic_cast<const ImportedEnum*>(symbol.getImportedNode())) {
                    return importedEnum->getGenericParameters().size();
                }
                if (const auto* importedTypeAlias = dynamic_cast<const ImportedTypeAlias*>(symbol.getImportedNode())) {
                    return importedTypeAlias->getGenericParameters().size();
                }
            }

            return 0;
        };

        const auto substituteImportedTypeParameters = [](std::string_view fullTypeName, const TypeSubstitutionMap& substitutions) -> std::string {
            std::string result;
            std::size_t tokenStart = 0;
            const auto appendToken = [&](std::string_view token) {
                if (token.empty()) {
                    return;
                }

                for (const auto& [parameter, argument] : substitutions) {
                    if (token == parameter) {
                        if (argument != nullptr) {
                            result += argument->getFullTypeName();
                        }
                        return;
                    }
                }

                result += token;
            };

            for (std::size_t index = 0; index < fullTypeName.size(); ++index) {
                const char character = fullTypeName[index];
                if (std::string_view("<>, \t\r\n").find(character) == std::string_view::npos) {
                    continue;
                }

                appendToken(fullTypeName.substr(tokenStart, index - tokenStart));
                if (character == '<' || character == '>') {
                    result += character;
                } else if (character == ',') {
                    result += ", ";
                }
                tokenStart = index + 1;
            }

            appendToken(fullTypeName.substr(tokenStart));
            return result;
        };

        const auto constructFullTypeName = [&](const CustomizedTypeReferenceNode& type, const std::vector<const Type*>& resolvedGenericArguments) -> std::string {
            const auto& nameParts = type.getNameParts();
            std::stack<std::string> identifiers;
            std::string result;

            const Scope* currentScope = &context.currentScope();
            while (currentScope && !currentScope->lookupLocal(nameParts.front()->getIdentifierString())) {
                currentScope = currentScope->findParent();
            }
            const auto* firstSymbol = dynamic_cast<const RegularSymbol*>(currentScope->lookupLocal(nameParts.front()->getIdentifierString()));

            const RegularSymbol* referencedSymbol = firstSymbol;
            if (nameParts.size() > 1) {
                const Scope* referencedScope = context.getScopeByRegularSymbol(*firstSymbol);
                for (std::size_t index = 1; index < nameParts.size(); ++index) {
                    referencedSymbol = dynamic_cast<const RegularSymbol*>(referencedScope->lookup(nameParts[index]->getIdentifierString()));
                    if (index + 1 < nameParts.size()) {
                        referencedScope = context.getScopeByRegularSymbol(*referencedSymbol);
                    }
                }
            }

            if (firstSymbol->getKind() == RegularSymbolKind::GENERIC_PARAMETER) {
                result = std::string(firstSymbol->getName());
            } else if (firstSymbol->getOrigin() == RegularSymbolOrigin::LOCAL) {
                const Scope* localScope = context.getScopeByRegularSymbol(*firstSymbol);
                if (localScope != nullptr) {
                    while (localScope->findParent() && localScope->getKind() != ScopeKind::MODULE) {
                        const TypeDeclarationNode* node = dynamic_cast<const TypeDeclarationNode*>(localScope->getLocalNode());
                        if (const ClassDeclarationNode* classDecl = dynamic_cast<const ClassDeclarationNode*>(node)) {
                            identifiers.push(std::string(classDecl->getName().getIdentifierString()));
                        } else if (const InterfaceDeclarationNode* interfaceDecl = dynamic_cast<const InterfaceDeclarationNode*>(node)) {
                            identifiers.push(std::string(interfaceDecl->getName().getIdentifierString()));
                        } else if (const EnumDeclarationNode* enumDecl = dynamic_cast<const EnumDeclarationNode*>(node)) {
                            identifiers.push(std::string(enumDecl->getName().getIdentifierString()));
                        } else if (const EnumMemberDeclarationNode* enumMemberDecl = dynamic_cast<const EnumMemberDeclarationNode*>(node)) {
                            identifiers.push(std::string(enumMemberDecl->getName().getIdentifierString()));
                        } else if (const TypeAliasDeclarationNode* typeAliasDecl = dynamic_cast<const TypeAliasDeclarationNode*>(node)) {
                            identifiers.push(std::string(typeAliasDecl->getAliasName().getIdentifierString()));
                        }

                        localScope = localScope->findParent();
                    }
                }

                result = context.getModule().getFullName();
                while (!identifiers.empty()) {
                    result.append(".");
                    result.append(identifiers.top());
                    identifiers.pop();
                }
                for (std::size_t index = 1; index < nameParts.size(); ++index) {
                    result.append(".");
                    result.append(nameParts[index]->getIdentifierString());
                }
            } else if (firstSymbol->getOrigin() == RegularSymbolOrigin::IMPORTED) {
                const Scope* importedScope = context.getScopeByRegularSymbol(*firstSymbol);
                if (importedScope == nullptr) {
                    context.reportError(*nameParts.front(), fmt::format("Identifier '{}' does not have a scope", nameParts.front()->getIdentifierString()));
                    return "";
                }
                while (importedScope != nullptr) {
                    if (const ImportedItem* importedNode = importedScope->getImportedNode()) {
                        identifiers.push(std::string(importedNode->getName()));
                    }
                    importedScope = importedScope->findParent();
                }
                if (identifiers.empty()) {
                    context.reportError(*nameParts.front(), fmt::format("Identifier '{}' does not have a scope", nameParts.front()->getIdentifierString()));
                    return "";
                }
                result = std::move(identifiers.top());
                identifiers.pop();
                while (!identifiers.empty()) {
                    result.append(".");
                    result.append(identifiers.top());
                    identifiers.pop();
                }
                for (std::size_t index = 1; index < nameParts.size(); ++index) {
                    result.append(".");
                    result.append(nameParts[index]->getIdentifierString());
                }
            }

            if (!resolvedGenericArguments.empty()) {
                std::string argumentList = "<";
                for (const auto* genericArgument : resolvedGenericArguments) {
                    argumentList += genericArgument->getFullTypeName();
                    argumentList += ", ";
                }
                argumentList.erase(argumentList.size() - 2);
                argumentList += '>';
                result += argumentList;
            }

            return result;
        };

        std::function<const Type*(const TypeReferenceNode&, const TypeSubstitutionMap&, VisitedSet)> checkType;
        const auto checkUnwrappedType = [&](const TypeReferenceNode& type, const TypeSubstitutionMap& substitutions, const VisitedSet& visited) -> const Type* {
            if (const auto* primitiveTypeReferenceNode = dynamic_cast<const PrimitiveTypeReferenceNode*>(&type)) {
                return getPrimitiveType(primitiveTypeReferenceNode->getKind());
            }

            const auto* customizedTypeReferenceNode = dynamic_cast<const CustomizedTypeReferenceNode*>(&type);
            if (customizedTypeReferenceNode == nullptr) {
                return nullptr;
            }

            const auto& nameParts = customizedTypeReferenceNode->getNameParts();
            if (nameParts.size() == 1) {
                const auto substitution = substitutions.find(std::string(nameParts.front()->getIdentifierString()));
                if (substitution != substitutions.end()) {
                    return substitution->second;
                }
            }

            const auto isTypeDefinition = [](RegularSymbolKind kind) {
                switch (kind) {
                    case RegularSymbolKind::CLASS:
                    case RegularSymbolKind::INTERFACE:
                    case RegularSymbolKind::ENUM:
                    case RegularSymbolKind::ENUM_MEMBER:
                    case RegularSymbolKind::TYPE_ALIAS:
                    case RegularSymbolKind::GENERIC_PARAMETER:
                        return true;
                    default:
                        return false;
                }
            };

            bool hasResolvedType = false;
            const RegularSymbol* typeSymbol = nullptr;

            const Scope* scope = &context.currentScope();
            for (std::size_t index = 0; index < nameParts.size(); ++index) {
                const auto& namePart = nameParts[index];
                const Symbol* symbol = scope->lookup(namePart->getIdentifierString());
                if (symbol == nullptr) {
                    context.reportError(*namePart, fmt::format("Use of undeclared type '{}'", namePart->getIdentifierString()));
                    break;
                }
                if (dynamic_cast<const FunctionSymbol*>(symbol)) {
                    context.reportError(*namePart, "Using function symbols in a type reference is not allowed");
                    break;
                }

                const RegularSymbol* regularSymbol = dynamic_cast<const RegularSymbol*>(symbol);
                if (index == nameParts.size() - 1) {
                    if (!isTypeDefinition(regularSymbol->getKind())) {
                        context.reportError(*namePart, fmt::format("Identifier '{}' is not a type definition", namePart->getIdentifierString()));
                    } else {
                        typeSymbol = regularSymbol;
                        hasResolvedType = true;
                    }
                    break;
                }

                scope = context.getScopeByRegularSymbol(*regularSymbol);
                if (scope == nullptr) {
                    context.reportError(*namePart, fmt::format("Identifier '{}' does not have a scope", namePart->getIdentifierString()));
                    break;
                }
            }

            if (!hasResolvedType) {
                return nullptr;
            }

            const std::size_t genericArgumentCount = customizedTypeReferenceNode->getGenericArguments().size();
            const std::size_t genericParameterCount = getGenericParameterCount(*typeSymbol);
            if (genericArgumentCount != genericParameterCount) {
                context.reportError(type, fmt::format("Generic argument count mismatch: expected {}, got {}", genericParameterCount, genericArgumentCount));
                return nullptr;
            }

            std::vector<const Type*> genericArgumentTypes;
            std::size_t errors = context.getErrors().size();
            for (const auto& genericArgument : customizedTypeReferenceNode->getGenericArguments()) {
                const Type* genericArgumentType = checkType(*genericArgument, substitutions, visited);

                if (genericArgumentType == nullptr) {
                    return nullptr;
                }
                genericArgumentTypes.emplace_back(genericArgumentType);
            }
            if (context.getErrors().size() > errors) {
                return nullptr;
            }

            if (typeSymbol->getKind() == RegularSymbolKind::TYPE_ALIAS) {
                if (typeSymbol->getOrigin() == RegularSymbolOrigin::LOCAL) {
                    const auto* typeAliasDeclaration = dynamic_cast<const TypeAliasDeclarationNode*>(typeSymbol->getLocalNode());
                    if (typeAliasDeclaration == nullptr) {
                        return nullptr;
                    }

                    TypeSubstitutionMap aliasSubstitutions;
                    const auto& genericParameterNames = typeAliasDeclaration->getGenericParameterNames();
                    for (std::size_t index = 0; index < genericParameterNames.size(); ++index) {
                        aliasSubstitutions.emplace(std::string(genericParameterNames[index]->getIdentifierString()), genericArgumentTypes[index]);
                    }

                    return checkType(typeAliasDeclaration->getOriginalType(), aliasSubstitutions, visited);
                } else if (typeSymbol->getOrigin() == RegularSymbolOrigin::IMPORTED) {
                    const auto* importedTypeAlias = dynamic_cast<const ImportedTypeAlias*>(typeSymbol->getImportedNode());
                    if (importedTypeAlias == nullptr) {
                        return nullptr;
                    }

                    TypeSubstitutionMap aliasSubstitutions;
                    const auto& genericParameterNames = importedTypeAlias->getGenericParameters();
                    for (std::size_t index = 0; index < genericParameterNames.size(); ++index) {
                        aliasSubstitutions.emplace(std::string(genericParameterNames[index]), genericArgumentTypes[index]);
                    }

                    std::string originalName = substituteImportedTypeParameters(importedTypeAlias->getOriginalType(), aliasSubstitutions);
                    registerImportedTypeReference(originalName);
                    return context.getTypeByFullTypeName(originalName);
                }
            }

            const std::string fullName = constructFullTypeName(dynamic_cast<const CustomizedTypeReferenceNode&>(type), genericArgumentTypes);

            const auto customizedKind = [&typeSymbol]() -> std::optional<CustomizedTypeKind> {
                switch (typeSymbol->getKind()) {
                    case RegularSymbolKind::CLASS:
                        return CustomizedTypeKind::CLASS;
                    case RegularSymbolKind::INTERFACE:
                        return CustomizedTypeKind::INTERFACE;
                    case RegularSymbolKind::ENUM:
                        return CustomizedTypeKind::ENUM;
                    case RegularSymbolKind::ENUM_MEMBER:
                        return CustomizedTypeKind::ENUM_MEMBER;
                    case RegularSymbolKind::GENERIC_PARAMETER:
                        return CustomizedTypeKind::GENERIC_PARAMETER;
                    default:
                        return std::nullopt;
                }
            }();
            if (!customizedKind.has_value()) {
                return nullptr;
            }

            const CustomizedType* existingCustomizedType = context.getCustomizedTypeByFullTypeName(fullName);
            if (existingCustomizedType != nullptr) {
                return existingCustomizedType;
            }

            std::unique_ptr<CustomizedType> customizedType;
            if (const auto* localNode = dynamic_cast<const TypeDeclarationNode*>(typeSymbol->getLocalNode())) {
                customizedType = std::make_unique<CustomizedType>(customizedKind.value(), fullName, std::move(genericArgumentTypes), localNode);
            } else if (const auto* importedNode = dynamic_cast<const ImportedIdentifier*>(typeSymbol->getImportedNode())) {
                customizedType = std::make_unique<CustomizedType>(customizedKind.value(), fullName, std::move(genericArgumentTypes), importedNode);
            } else {
                return nullptr;
            }

            const Type* resolvedType = customizedType.get();
            context.registerCustomizedType(std::move(customizedType));
            return resolvedType;
        };

        checkType = [&](const TypeReferenceNode& type, const TypeSubstitutionMap& substitutions, VisitedSet visited) -> const Type* {
            if (!visited.insert(&type).second) {
                context.reportError(type, "Cyclic type reference detected");
                return nullptr;
            }

            if (substitutions.empty()) {
                const Type* existedType = context.getTypeByTypeReferenceNode(&type);
                if (existedType) {
                    return existedType; // avoid resolving repeatitively
                }
            }

            const Type* resolvedType = checkUnwrappedType(type, substitutions, visited);
            if (resolvedType == nullptr) {
                return nullptr;
            }

            if (type.hasQuestionMarkSuffix()) {
                const std::string fullName = StandardLibraryTypeNames::Optional(resolvedType->getFullTypeName());
                const CustomizedType* optionalType = context.getCustomizedTypeByFullTypeName(fullName);

                if (optionalType == nullptr) {
                    std::unique_ptr<CustomizedType> customizedType = nullptr;
                    const auto* currModule = &context.getModule();

                    if (currModule->getFullName() == "vanillang.typesystem") {
                        const EnumDeclarationNode* opt = nullptr;
                        for (const auto& decl : currModule->getTopIdentifierDeclarations()) {
                            const EnumDeclarationNode* enumDecl = dynamic_cast<const EnumDeclarationNode*>(decl.get());

                            if (enumDecl && enumDecl->getName().getIdentifierString() == "Optional") {
                                opt = enumDecl;
                                break;
                            }
                        }

                        if (!opt) {
                            context.reportError(type, "Could not find the declaration of 'vanillang.typesystem.Optional'");
                            return nullptr;
                        }

                        customizedType = std::make_unique<CustomizedType>(CustomizedTypeKind::ENUM, fullName, std::vector<const Type*>{ resolvedType }, opt);
                    } else {
                        const ImportedItem* item = context.getImportedItemByFullPath({ "vanillang", "typesystem", "Optional" });
                        const ImportedEnum* opt = dynamic_cast<const ImportedEnum*>(item);

                        if (!opt) {
                            context.reportError(type, "Could not find the declaration of 'vanillang.typesystem.Optional'");
                            return nullptr;
                        }

                        customizedType = std::make_unique<CustomizedType>(CustomizedTypeKind::ENUM, fullName, std::vector<const Type*>{ resolvedType }, opt);
                    }

                    optionalType = customizedType.get();
                    context.registerCustomizedType(std::move(customizedType));
                }

                resolvedType = optionalType;
            }

            if (substitutions.empty()) {
                context.mapType(&type, resolvedType);
            }
            return resolvedType;
        };

        checkType(typeRef, TypeSubstitutionMap{}, {});
    }

    void SemanticAnalyzer::resolveTypeReferencesInModule(const ModuleNode& moduleNode) {
        for (const auto& topIdentifierDecl : moduleNode.getTopIdentifierDeclarations()) {
            const DeclarationNode* declNode = topIdentifierDecl.get();

            if (const auto* varDecl = dynamic_cast<const ValueDeclarationNode*>(declNode)) {
                if (varDecl->getType().has_value()) {
                    resolveTypeReference(*varDecl->getType().value());
                }
            } else if (const auto* funcDecl = dynamic_cast<const FunctionDeclarationNode*>(declNode)) {
                resolveTypeReferencesInFunction(*funcDecl);
            } else if (const auto* classDecl = dynamic_cast<const ClassDeclarationNode*>(declNode)) {
                resolveTypeReferencesInClass(*classDecl);
            } else if (const auto* interfaceDecl = dynamic_cast<const InterfaceDeclarationNode*>(declNode)) {
                resolveTypeReferencesInInterface(*interfaceDecl);
            } else if (const auto* enumDecl = dynamic_cast<const EnumDeclarationNode*>(declNode)) {
                resolveTypeReferencesInEnum(*enumDecl);
            } else if (const auto* typeAliasDecl = dynamic_cast<const TypeAliasDeclarationNode*>(declNode)) {
                resolveTypeReferencesInTypeAlias(*typeAliasDecl);
            }
        }
    }

    void SemanticAnalyzer::resolveTypeReferencesInFunction(const FunctionDeclarationNode& functionNode) {
        if (!context.pushExistingScope(&functionNode)) {
            return;
        }

        for (const auto& parameter : functionNode.getParameters()) {
            if (parameter->getType().has_value()) {
                resolveTypeReference(*parameter->getType().value());
            }
        }

        if (functionNode.getReturnType().has_value()) {
            resolveTypeReference(*functionNode.getReturnType().value());
        }

        context.popScope();
    }

    void SemanticAnalyzer::resolveTypeReferencesInConstructor(const ConstructorDeclarationNode& constructorNode) {
        if (!context.pushExistingScope(&constructorNode)) {
            return;
        }

        for (const auto& parameter : constructorNode.getParameters()) {
            if (parameter->getType().has_value()) {
                resolveTypeReference(*parameter->getType().value());
            }
        }

        context.popScope();
    }

    void SemanticAnalyzer::resolveTypeReferencesInOperator(const OperatorDeclarationNode& operatorNode) {
        if (!context.pushExistingScope(&operatorNode)) {
            return;
        }

        for (const auto& parameter : operatorNode.getParameters()) {
            if (parameter->getType().has_value()) {
                resolveTypeReference(*parameter->getType().value());
            }
        }

        if (operatorNode.getReturnType().has_value()) {
            resolveTypeReference(*operatorNode.getReturnType().value());
        }

        context.popScope();
    }

    void SemanticAnalyzer::resolveTypeReferencesInClass(const ClassDeclarationNode& classNode) {
        if (!context.pushExistingScope(&classNode)) {
            return;
        }

        if (classNode.getBaseClass().has_value()) {
            resolveTypeReference(*classNode.getBaseClass().value());
        }

        for (const auto& interfaceRefNode : classNode.getImplementedInterfaces()) {
            resolveTypeReference(*interfaceRefNode);
        }

        for (const auto& propertyDecl : classNode.getPropertyDeclarations()) {
            if (propertyDecl->getType().has_value()) {
                resolveTypeReference(*propertyDecl->getType().value());
            }
        }

        for (const auto& methodDecl : classNode.getMethodDeclarations()) {
            resolveTypeReferencesInFunction(*methodDecl);
        }

        for (const auto& constructorDecl : classNode.getConstructorDeclarations()) {
            resolveTypeReferencesInConstructor(*constructorDecl);
        }

        for (const auto& operatorDecl : classNode.getOperatorDeclarations()) {
            resolveTypeReferencesInOperator(*operatorDecl);
        }

        context.popScope();
    }

    void SemanticAnalyzer::resolveTypeReferencesInInterface(const InterfaceDeclarationNode& interfaceNode) {
        if (!context.pushExistingScope(&interfaceNode)) {
            return;
        }

        for (const auto& methodDecl : interfaceNode.getMethodDeclarations()) {
            resolveTypeReferencesInFunction(*methodDecl);
        }

        for (const auto& operatorDecl : interfaceNode.getOperatorDeclarations()) {
            resolveTypeReferencesInOperator(*operatorDecl);
        }

        context.popScope();
    }

    void SemanticAnalyzer::resolveTypeReferencesInEnum(const EnumDeclarationNode& enumNode) {
        if (!context.pushExistingScope(&enumNode)) {
            return;
        }

        for (const auto& member : enumNode.getMemberDeclarations()) {
            resolveTypeReferencesInEnumMember(*member);
        }

        context.popScope();
    }

    void SemanticAnalyzer::resolveTypeReferencesInEnumMember(const EnumMemberDeclarationNode& enumMemberNode) {
        if (!context.pushExistingScope(&enumMemberNode)) {
            return;
        }

        for (const auto& associatedValue : enumMemberNode.getAssociatedValues()) {
            if (associatedValue->getType().has_value()) {
                resolveTypeReference(*associatedValue->getType().value());
            }
        }

        context.popScope();
    }

    void SemanticAnalyzer::resolveTypeReferencesInTypeAlias(const TypeAliasDeclarationNode& typeAliasNode) {
        if (!context.pushExistingScope(&typeAliasNode)) {
            return;
        }

        resolveTypeReference(typeAliasNode.getOriginalType());

        context.popScope();
    }

    void SemanticAnalyzer::generateInternalName(const DeclarationNode& node) {
        std::string name;

        if (const FunctionDeclarationNode* functionNode = dynamic_cast<const FunctionDeclarationNode*>(&node)) {
            name.append("vnl-function-");
            name.append((functionNode->getName().getIdentifierString()));
            name.append("(");
            for (const auto& param : functionNode->getParameters()) {
                const Type* semanticType = context.getTypeByTypeReferenceNode(param->getType()->get());
                if (!semanticType) return;

                name.append(semanticType->getFullTypeName());
                name.append(", ");
            }
            if (name.ends_with(", ")) name.erase(name.size() - 2);
            name.append(")");

            functionNode->setInternalName(name);
        } else if (const ConstructorDeclarationNode* constructorNode = dynamic_cast<const ConstructorDeclarationNode*>(&node)) {
            name.append("vnl-constructor");
            name.append("(");
            for (const auto& param : constructorNode->getParameters()) {
                const Type* semanticType = context.getTypeByTypeReferenceNode(param->getType()->get());
                if (!semanticType) return;

                name.append(semanticType->getFullTypeName());
                name.append(", ");
            }
            if (name.ends_with(", ")) name.erase(name.size() - 2);
            name.append(")");

            constructorNode->setInternalName(name);
        } else if (const OperatorDeclarationNode* operatorNode = dynamic_cast<const OperatorDeclarationNode*>(&node)) {
            name.append("vnl-operator-");
            name.append((operatorNode->getOperatorName()));
            name.append("(");
            for (const auto& param : operatorNode->getParameters()) {
                const Type* semanticType = context.getTypeByTypeReferenceNode(param->getType()->get());
                if (!semanticType) return;

                name.append(semanticType->getFullTypeName());
                name.append(", ");
            }
            if (name.ends_with(", ")) name.erase(name.size() - 2);
            name.append(")");

            operatorNode->setInternalName(name);
        }
    }

    void SemanticAnalyzer::generateInternalNameForOverloadablesInModule(const ModuleNode& moduleNode) {
        for (const auto& topIdentifierDecl : moduleNode.getTopIdentifierDeclarations()) {
            const DeclarationNode* declNode = topIdentifierDecl.get();

            if (const auto* funcDecl = dynamic_cast<const FunctionDeclarationNode*>(declNode)) {
                generateInternalName(*funcDecl);
            } else if (const auto* classDecl = dynamic_cast<const ClassDeclarationNode*>(declNode)) {
                generateInternalNameForOverloadablesInClass(*classDecl);
            } else if (const auto* interfaceDecl = dynamic_cast<const InterfaceDeclarationNode*>(declNode)) {
                generateInternalNameForOverloadablesInInterface(*interfaceDecl);
            }
        }
    }

    void SemanticAnalyzer::generateInternalNameForOverloadablesInClass(const ClassDeclarationNode& classNode) {
        for (const auto& methodDecl : classNode.getMethodDeclarations()) {
            generateInternalName(*methodDecl);
        }

        for (const auto& constructorDecl : classNode.getConstructorDeclarations()) {
            generateInternalName(*constructorDecl);
        }

        for (const auto& operatorDecl : classNode.getOperatorDeclarations()) {
            generateInternalName(*operatorDecl);
        }
    }

    void SemanticAnalyzer::generateInternalNameForOverloadablesInInterface(const InterfaceDeclarationNode& interfaceNode) {
        for (const auto& methodDecl : interfaceNode.getMethodDeclarations()) {
            generateInternalName(*methodDecl);
        }

        for (const auto& operatorDecl : interfaceNode.getOperatorDeclarations()) {
            generateInternalName(*operatorDecl);
        }
    }

    void SemanticAnalyzer::collectLocalFunctionSymbolsInModule(const ModuleNode& moduleNode) {
        for (const auto& topIdentifierDecl : moduleNode.getTopIdentifierDeclarations()) {
            const DeclarationNode* declNode = topIdentifierDecl.get();

            if (const auto* funcDecl = dynamic_cast<const FunctionDeclarationNode*>(declNode)) {
                declareFunctionOverloading(*funcDecl, RegularSymbolKind::FUNCTION, "symbol");
            } else if (const auto* classDecl = dynamic_cast<const ClassDeclarationNode*>(declNode)) {
                collectLocalFunctionSymbolsInClass(*classDecl);
            } else if (const auto* interfaceDecl = dynamic_cast<const InterfaceDeclarationNode*>(declNode)) {
                collectLocalFunctionSymbolsInInterface(*interfaceDecl);
            }
        }
    }

    void SemanticAnalyzer::collectLocalFunctionSymbolsInClass(const ClassDeclarationNode& classNode) {
        if (!context.pushExistingScope(&classNode)) {
            return;
        }
        std::unordered_set<std::string> operatorNames;
        std::unordered_set<std::string> constructorNames;

        for (const auto& constructorDecl : classNode.getConstructorDeclarations()) {
            if (!constructorNames.insert(std::string(constructorDecl->getInternalName())).second) {
                context.reportError(*constructorDecl, fmt::format("Redeclaration of class member '{}'", constructorDecl->getInternalName()));
            }
        }

        for (const auto& operatorDecl : classNode.getOperatorDeclarations()) {
            if (!operatorNames.insert(std::string(operatorDecl->getInternalName())).second) {
                context.reportError(*operatorDecl, fmt::format("Redeclaration of class member '{}'", operatorDecl->getInternalName()));
            }
        }

        for (const auto& methodDecl : classNode.getMethodDeclarations()) {
            declareFunctionOverloading(*methodDecl, RegularSymbolKind::METHOD, "class member");
        }

        context.popScope();
    }

    void SemanticAnalyzer::collectLocalFunctionSymbolsInInterface(const InterfaceDeclarationNode& interfaceNode) {
        if (!context.pushExistingScope(&interfaceNode)) {
            return;
        }
        std::unordered_set<std::string> operatorNames;

        for (const auto& operatorDecl : interfaceNode.getOperatorDeclarations()) {
            if (!operatorNames.insert(std::string(operatorDecl->getInternalName())).second) {
                context.reportError(*operatorDecl, fmt::format("Redeclaration of interface operator '{}'", operatorDecl->getInternalName()));
            }
        }

        for (const auto& methodDecl : interfaceNode.getMethodDeclarations()) {
            declareFunctionOverloading(*methodDecl, RegularSymbolKind::METHOD, "interface method");
        }

        context.popScope();
    }

    void SemanticAnalyzer::buildInheritanceGraph(const ModuleNode& localModule, const std::vector<const ImportedModule*>& importedModules) {
        for (const auto& topIdentifierDecl : localModule.getTopIdentifierDeclarations()) {
            if (const auto* interfaceDecl = dynamic_cast<const InterfaceDeclarationNode*>(topIdentifierDecl.get())) {
                context.getOrCreateLocalInheritanceNode(*interfaceDecl);
                continue;
            }

            if (const auto* enumDecl = dynamic_cast<const EnumDeclarationNode*>(topIdentifierDecl.get())) {
                const EnumInheritanceNode& enumNode = context.getOrCreateLocalInheritanceNode(*enumDecl);
                for (const auto& memberDecl : enumDecl->getMemberDeclarations()) {
                    context.getOrCreateLocalInheritanceNode(*memberDecl).setParent(&enumNode);
                }
                continue;
            }

            const auto* classDecl = dynamic_cast<const ClassDeclarationNode*>(topIdentifierDecl.get());
            if (classDecl == nullptr) {
                continue;
            }

            ClassInheritanceNode& classNode = context.getOrCreateLocalInheritanceNode(*classDecl);

            if (classDecl->getBaseClass().has_value()) {
                const auto* baseClassType = dynamic_cast<const CustomizedType*>(context.getTypeByTypeReferenceNode(classDecl->getBaseClass().value().get()));
                if (baseClassType && baseClassType->getCustomizedKind() == CustomizedTypeKind::CLASS) {
                    if (const auto* localNode = dynamic_cast<const ClassDeclarationNode*>(baseClassType->getLocalNode())) {
                        classNode.setParent(&context.getOrCreateLocalInheritanceNode(*localNode));
                    } else if (const auto* importedNode = dynamic_cast<const ImportedClass*>(baseClassType->getImportedNode())) {
                        classNode.setParent(&context.getOrCreateImportedInheritanceNode(*importedNode));
                    }
                }
            }

            for (const auto& implementedInterface : classDecl->getImplementedInterfaces()) {
                const auto* interfaceType = dynamic_cast<const CustomizedType*>(context.getTypeByTypeReferenceNode(implementedInterface.get()));
                if (interfaceType == nullptr) {
                    continue;
                }

                if (const auto* localNode = dynamic_cast<const InterfaceDeclarationNode*>(interfaceType->getLocalNode())) {
                    classNode.addImplementedInterface(&context.getOrCreateLocalInheritanceNode(*localNode));
                } else if (const auto* importedNode = dynamic_cast<const ImportedInterface*>(interfaceType->getImportedNode())) {
                    classNode.addImplementedInterface(&context.getOrCreateImportedInheritanceNode(*importedNode));
                }
            }
        }

        for (const ImportedModule* importedModule : importedModules) {
            for (const auto& [name, identifier] : importedModule->getIdentifiers()) {
                if (const auto* importedInterface = dynamic_cast<const ImportedInterface*>(identifier.get())) {
                    context.getOrCreateImportedInheritanceNode(*importedInterface);
                    continue;
                }

                if (const auto* importedEnum = dynamic_cast<const ImportedEnum*>(identifier.get())) {
                    const EnumInheritanceNode& enumNode = context.getOrCreateImportedInheritanceNode(*importedEnum);
                    for (const auto& [memberName, member] : importedEnum->getMembers()) {
                        context.getOrCreateImportedInheritanceNode(*member).setParent(&enumNode);
                    }
                    continue;
                }

                const auto* importedClass = dynamic_cast<const ImportedClass*>(identifier.get());
                if (importedClass == nullptr) {
                    continue;
                }

                ClassInheritanceNode& classNode = context.getOrCreateImportedInheritanceNode(*importedClass);
                if (importedClass->getBaseClass().has_value()) {
                    if (const auto* baseClass = dynamic_cast<const ImportedClass*>(context.getImportedItemByFullTypeName(importedClass->getBaseClass().value()))) {
                        classNode.setParent(&context.getOrCreateImportedInheritanceNode(*baseClass));
                    }
                }

                for (const auto& implementedInterface : importedClass->getImplementedInterfaces()) {
                    if (const auto* interfaceNode = dynamic_cast<const ImportedInterface*>(context.getImportedItemByFullTypeName(implementedInterface))) {
                        classNode.addImplementedInterface(&context.getOrCreateImportedInheritanceNode(*interfaceNode));
                    }
                }
            }
        }
    }

    MetadataInfo SemanticAnalyzer::checkMetadata(const std::vector<DeclarationItem::MetadataTerm>& metadataTerms, const DeclarationNode& declNode) {
        bool noWarnings = false;
        bool deprecated = false;

        for (const auto& term : metadataTerms) {
            if (term.key->getIdentifierString() == "nowarnings") {
                noWarnings = true;
            } else if (term.key->getIdentifierString() == "deprecated") {
                deprecated = true;
            } else {
                context.reportError(declNode, fmt::format("Unknown metadata term '{}'", term.key->getIdentifierString()));
            }
        }

        return MetadataInfo{
            noWarnings,
            deprecated,
        };
    }

    void SemanticAnalyzer::checkModule(const ModuleNode& moduleNode, const Config& config) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::MODULE, nullptr, &moduleNode));

        for (const auto& importDecl : moduleNode.getImportDeclarations()) {
            checkImport(*importDecl, config);
        }

        collectLocalRegularSymbolsInModule(moduleNode);
        resolveTypeReferencesInModule(moduleNode);
        generateInternalNameForOverloadablesInModule(moduleNode);
        collectLocalFunctionSymbolsInModule(moduleNode);

        std::function<void(const ImportedItem*, std::vector<const ImportedModule*>&)> collectModuleNode = [&](const ImportedItem* node, std::vector<const ImportedModule*>& importedModules) {
            if (const ImportedPackage* packageNode = dynamic_cast<const ImportedPackage*>(node)) {
                for (const auto& child : packageNode->getSubPackages()) {
                    collectModuleNode(child.second.get(), importedModules);
                }

                for (const auto& child : packageNode->getModules()) {
                    collectModuleNode(child.second.get(), importedModules);
                }
            } else if (const ImportedModule* moduleNode = dynamic_cast<const ImportedModule*>(node)) {
                importedModules.emplace_back(moduleNode);
            }
        };

        std::vector<const ImportedModule*> importedModules;
        for (const auto& item : context.getImports()) {
            const ImportedItem* node = item.second.get();
            collectModuleNode(node, importedModules);
        }
        buildInheritanceGraph(moduleNode, importedModules);

        for (const auto& topIdentifierDecl : moduleNode.getTopIdentifierDeclarations()) {
            DeclarationNode* declNode = topIdentifierDecl.get();

            if (auto* varDecl = dynamic_cast<ValueDeclarationNode*>(declNode)) {
                checkValueDeclaration(*varDecl);
            } else if (auto* funcDecl = dynamic_cast<FunctionDeclarationNode*>(declNode)) {
                checkFunctionDeclaration(*funcDecl);
            } else if (auto* classDecl = dynamic_cast<ClassDeclarationNode*>(declNode)) {
                checkClassDeclaration(*classDecl, config);
            } else if (auto* interfaceDecl = dynamic_cast<InterfaceDeclarationNode*>(declNode)) {
                checkInterfaceDeclaration(*interfaceDecl, config);
            } else if (auto* enumDecl = dynamic_cast<EnumDeclarationNode*>(declNode)) {
                checkEnumDeclaration(*enumDecl, config);
            }
        }

        for (const auto& exportDecl : moduleNode.getExportDeclarations()) {
            checkExport(*exportDecl);
        }

        context.popScope();
    }

    void SemanticAnalyzer::checkImport(const ImportDeclarationNode& importDecl, const Config&) {
        const auto& importItem = importDecl.getNamePartsListWithAliases();

        struct ImportBinding {
            std::string name;
            std::vector<std::string> path;
        };

        std::vector<ImportBinding> bindings;
        std::vector<const ImportedItem*> importedRoots;
        std::unordered_map<std::string, bool> names;
        const auto errorCount = context.getErrors().size();

        const auto bind = [&](const ImportedItem* target, std::vector<std::string> path, const IdentifierNode* alias, const AstNode& location) {
            std::string name;
            if (alias != nullptr) {
                name = alias->getIdentifierString();
            } else if (const auto* function = dynamic_cast<const ImportedFunc*>(target)) {
                name = function->getOriginalName();
            } else if (const auto* method = dynamic_cast<const ImportedMethod*>(target)) {
                name = method->getOriginalName();
            } else {
                name = target->getName();
            }

            std::unordered_set<const ImportedAlias*> visitedAliases;
            while (const auto* importedAlias = dynamic_cast<const ImportedAlias*>(target)) {
                if (!visitedAliases.insert(importedAlias).second) {
                    context.reportError(location, fmt::format("Cyclic imported alias '{}'", importedAlias->getSource()));
                    return;
                }

                path.assign(1, std::string());
                for (const char ch : importedAlias->getSource()) {
                    if (ch == '.') {
                        path.emplace_back();
                    } else {
                        path.back().push_back(ch);
                    }
                }

                target = context.getImportedItemByFullPath(path);
                if (target == nullptr) {
                    context.reportError(location, fmt::format("Could not find imported alias target '{}'", importedAlias->getSource()));
                    return;
                }
            }

            const bool isFunction = dynamic_cast<const ImportedFunc*>(target) != nullptr || dynamic_cast<const ImportedMethod*>(target) != nullptr;

            const Symbol* existingSymbol = context.currentScope().lookupLocal(name);
            if (existingSymbol != nullptr && !(isFunction && dynamic_cast<const FunctionSymbol*>(existingSymbol) != nullptr)) {
                context.reportError(location, fmt::format("Redeclaration of symbol '{}'", name));
                return;
            }

            auto [nameIterator, inserted] = names.try_emplace(name, isFunction);
            if (!inserted && !(nameIterator->second && isFunction)) {
                context.reportError(location, fmt::format("Redeclaration of symbol '{}'", name));
                return;
            }

            bindings.push_back({ name, std::move(path) });
        };

        std::function<void(const ImportDeclarationItem&, const ImportedItem*, std::vector<std::string>)> checkItem;
        checkItem = [&](const ImportDeclarationItem& item, const ImportedItem* parent, std::vector<std::string> path) {
            const AstNode& location = item.namePrefix.empty() ? static_cast<const AstNode&>(importDecl) : *item.namePrefix.back();
            const ImportedItem* target = parent;
            if (!item.self) {
                for (const auto& part : item.namePrefix) {
                    const auto name = part->getIdentifierString();
                    if (item.wildcard && name == "*") {
                        continue;
                    }
                    const ImportedItem* parentItem = target;
                    if (target == nullptr) {
                        target = context.getImportedRootPackageByName(name);
                    } else {
                        target = target->getChildByName(name);
                    }
                    if (target == nullptr) {
                        const auto* importedModule = dynamic_cast<const ImportedModule*>(parentItem);
                        if (importedModule != nullptr && &part == &item.namePrefix.back() && item.nameSuffixes.empty()) {
                            const IdentifierNode* alias = item.alias.has_value() ? item.alias.value().get() : nullptr;
                            bool matched = false;
                            for (const auto& [internalName, identifier] : importedModule->getIdentifiers()) {
                                const auto* function = dynamic_cast<const ImportedFunc*>(identifier.get());
                                const auto* method = dynamic_cast<const ImportedMethod*>(identifier.get());
                                const std::string_view originalName = function != nullptr ? function->getOriginalName() : method != nullptr ? method->getOriginalName() : std::string_view();
                                if (originalName != name) {
                                    continue;
                                }

                                matched = true;
                                auto functionPath = path;
                                functionPath.push_back(internalName);
                                bind(identifier.get(), std::move(functionPath), alias, alias ? *alias : location);
                            }
                            if (matched) {
                                return;
                            }
                        }
                        context.reportError(*part, fmt::format("Could not find imported package, module or identifier '{}'", name));
                        return;
                    }
                    path.emplace_back(name);
                }
            }

            if (target == nullptr) {
                context.reportError(location, "Import path must name a package, module or identifier");
                return;
            }
            if (item.self && !dynamic_cast<const ImportedPackage*>(target) && !dynamic_cast<const ImportedModule*>(target)) {
                context.reportError(location, "Self imports can only refer to packages or modules");
                return;
            }
            if (item.wildcard) {
                const auto* importedModule = dynamic_cast<const ImportedModule*>(target);
                if (importedModule == nullptr) {
                    context.reportError(location, "Wildcard imports can only be used for modules");
                    return;
                }
                if (item.alias.has_value()) {
                    context.reportError(*item.alias.value(), "Wildcard imports cannot have aliases");
                    return;
                }
                std::vector<std::string> identifierNames;
                for (const auto& [name, identifier] : importedModule->getIdentifiers()) {
                    identifierNames.push_back(name);
                }
                std::sort(identifierNames.begin(), identifierNames.end());
                for (const auto& name : identifierNames) {
                    path.push_back(name);
                    bind(importedModule->getIdentifierByName(name), path, nullptr, location);
                    path.pop_back();
                }
            } else if (!item.nameSuffixes.empty()) {
                for (const auto& suffix : item.nameSuffixes) {
                    checkItem(*suffix, target, path);
                }
            } else {
                const auto* alias = item.alias.has_value() ? item.alias.value().get() : nullptr;
                bind(target, path, alias, alias ? *alias : location);
            }
        };

        checkItem(importItem, nullptr, {});
        if (context.getErrors().size() != errorCount) {
            return;
        }

        std::unordered_set<const ImportedPackage*> scopedPackages;
        std::unordered_set<std::string> reportedRedeclarations;

        for (const auto& binding : bindings) {
            const auto* package = context.getImportedRootPackageByName(binding.path.front());
            const ImportedItem* target = context.getImportedItemByFullPath(binding.path);
            if (target == nullptr) {
                continue;
            }

            bool declared = false;
            if (const auto* function = dynamic_cast<const ImportedFunc*>(target)) {
                FunctionSymbol functionSymbol(binding.name);
                functionSymbol.addOverloading(RegularSymbol(RegularSymbolKind::FUNCTION, RegularSymbolAccessModifier::PUBLIC, function->getName(), function));
                declared = context.currentScope().declare(std::move(functionSymbol));
            } else if (const auto* method = dynamic_cast<const ImportedMethod*>(target)) {
                FunctionSymbol functionSymbol(binding.name);
                functionSymbol.addOverloading(RegularSymbol(RegularSymbolKind::METHOD, getImportedAccessModifier(*method), method->getName(), method));
                declared = context.currentScope().declare(std::move(functionSymbol));
            } else {
                declared = context.currentScope().declare(RegularSymbol(getImportedSymbolKind(*target), RegularSymbolAccessModifier::PUBLIC, binding.name, target));
            }
            if (!declared && reportedRedeclarations.insert(binding.name).second) {
                context.reportError(importDecl, fmt::format("Redeclaration of symbol '{}'", binding.name));
            }
            if (declared) {
                importedRoots.push_back(target);
            }
            if (getImportedScopeKind(*target).has_value()) {
                scopedPackages.insert(package);
            }
        }

        for (const auto* package : scopedPackages) {
            registerImportedScopes(*package, nullptr);
        }

        for (const auto* root : importedRoots) {
            registerImportedTypeReferences(*root);
        }
    }

    void SemanticAnalyzer::checkExport(const ExportDeclarationNode& exportDecl) {
        for (auto& item : exportDecl.getNameList()) {
            if (context.currentScope().lookup(item.name->getIdentifierString()) == nullptr) {
                context.reportError(exportDecl, fmt::format("Use of undeclared identifier {}", item.name->getIdentifierString()));
            }
        }
    }

    void SemanticAnalyzer::checkValueDeclaration(const ValueDeclarationNode& varDecl) {
        auto kind = varDecl.getKind();

        if (varDecl.getContext() == ValueDeclarationKind::Context::TOP_LEVEL) {
            if (varDecl.getKind() != ValueDeclarationKind::Kind::LET) {
                context.reportError(varDecl, "Top-level value declaration must be a variable");
            }
        }

        const bool hasTypeReference = varDecl.getType().has_value();
        const bool hasInitializer = varDecl.getInitializer().has_value();
        const bool requiresInitializer = kind == ValueDeclarationKind::Kind::LET || kind == ValueDeclarationKind::Kind::STATIC_PROPERTY;

        if (requiresInitializer && !hasInitializer) {
            context.reportError(varDecl, kind == ValueDeclarationKind::Kind::STATIC_PROPERTY ? "Static properties must be initialized" : "Variables must be initialized");
        } else if (!requiresInitializer && hasInitializer) {
            context.reportError(
                varDecl,
                kind == ValueDeclarationKind::Kind::INSTANCE_PROPERTY ? "Instance properties cannot have initializers" : "Value declarations of this kind cannot have initializers"
            );
        }

        if (hasInitializer) {
            checkExpression(*varDecl.getInitializer().value());
            const ExpressionAnalysisResult* exprResult = context.getExpressionAnalysisResult(varDecl.getInitializer()->get());

            if (exprResult && exprResult->valueType == nullptr) {
                context.reportError(*varDecl.getInitializer().value(), "Initializer expression does not have a value");
            }

            if (hasTypeReference) {
                const Type* exprType = exprResult ? exprResult->valueType : nullptr;
                const Type* declType = context.getTypeByTypeReferenceNode(varDecl.getType()->get());

                if (exprType && declType && !exprType->canConvertTo(*declType)) {
                    context.reportError(varDecl, fmt::format("Cannot convert {} to {}", exprType->getFullTypeName(), declType->getFullTypeName()));
                }
            }
        }
    }

    void SemanticAnalyzer::checkFunctionDeclaration(const FunctionDeclarationNode& funcDecl) {
        if (!context.pushExistingScope(&funcDecl)) {
            return;
        }

        const bool hasFunctionBody = funcDecl.getBody().has_value();

        for (const auto& param : funcDecl.getParameters()) {
            checkValueDeclaration(*param);
        }

        if (funcDecl.getContext() == FunctionDeclarationKind::Context::INTERFACE) {
            if (hasFunctionBody) {
                context.reportError(funcDecl, "Interface methods cannot have a body");
            }
        } else {
            if (funcDecl.getImplementation() == FunctionDeclarationKind::Implementation::NATIVE) {
                if (hasFunctionBody) {
                    context.reportError(funcDecl, "Native functions cannot have a body");
                }
            }

            if (funcDecl.getImplementation() == FunctionDeclarationKind::Implementation::REGULAR) {
                if (hasFunctionBody) {
                    checkStatement(*funcDecl.getBody().value());
                } else {
                    context.reportError(funcDecl, "Regular functions must have a body");
                }
            }
        }

        // TODO: Implement return type inference

        context.popScope();
    }

    void SemanticAnalyzer::checkConstructorDeclaration(const ConstructorDeclarationNode& constructorDecl) {
        if (!context.pushExistingScope(&constructorDecl)) {
            return;
        }

        for (const auto& param : constructorDecl.getParameters()) {
            checkValueDeclaration(*param);
        }

        checkStatement(constructorDecl.getBody());

        context.popScope();
    }

    void SemanticAnalyzer::checkOperatorDeclaration(const OperatorDeclarationNode& operatorDecl) {
        if (!context.pushExistingScope(&operatorDecl)) {
            return;
        }

        const bool hasBody = operatorDecl.getBody().has_value();

        for (const auto& param : operatorDecl.getParameters()) {
            checkValueDeclaration(*param);
        }

        if (operatorDecl.getContext() == OperatorDeclarationKind::Context::INTERFACE) {
            if (hasBody) {
                context.reportError(operatorDecl, "Interface operators cannot have a body");
            }
        } else {
            if (operatorDecl.getImplementation() == OperatorDeclarationKind::Implementation::NATIVE) {
                if (hasBody) {
                    context.reportError(operatorDecl, "Native operators cannot have a body");
                }
            }

            if (operatorDecl.getImplementation() == OperatorDeclarationKind::Implementation::REGULAR) {
                if (hasBody) {
                    checkStatement(*operatorDecl.getBody().value());
                } else {
                    context.reportError(operatorDecl, "Regular operators must have a body");
                }
            }
        }

        // TODO: Implement return type inference

        std::optional<std::string> requiredReturnTypeName;
        switch (operatorDecl.getKind()) {
            case OperatorDeclarationKind::Kind::EQUAL:
                requiredReturnTypeName = PrimitiveTypeNames::Boolean;
                break;
            case OperatorDeclarationKind::Kind::COMPARISON:
                requiredReturnTypeName = StandardLibraryTypeNames::Optional(StandardLibraryTypeNames::Ordering);
                break;
            default:
                break;
        }

        if (requiredReturnTypeName.has_value()) {
            const auto& returnTypeRefNode = operatorDecl.getReturnType();
            const std::string message = fmt::format("Operator '{}' must return '{}'", operatorDecl.getOperatorName(), requiredReturnTypeName.value());

            if (returnTypeRefNode.has_value()) {
                const Type* returnType = context.getTypeByTypeReferenceNode(returnTypeRefNode.value().get());
                if (returnType != nullptr && returnType->getFullTypeName() != requiredReturnTypeName.value()) {
                    context.reportError(*returnTypeRefNode.value(), message);
                }
            } else if (const Type* inferredReturnType = context.getInferredOperatorReturnType(operatorDecl)) {
                if (inferredReturnType->getFullTypeName() != requiredReturnTypeName.value()) {
                    context.reportError(operatorDecl, message);
                }
            } else if (!hasBody) {
                context.reportError(operatorDecl, message);
            }
        }

        context.popScope();
    }

    void SemanticAnalyzer::checkClassDeclaration(const ClassDeclarationNode& classDecl, const Config& config) {
        if (!context.pushExistingScope(&classDecl)) {
            return;
        }

        if (classDecl.getBaseClass().has_value()) {
            const TypeReferenceNode* baseClassRefNode = classDecl.getBaseClass().value().get();
            const Type* resolvedType = context.getTypeByTypeReferenceNode(baseClassRefNode);

            if (dynamic_cast<const PrimitiveType*>(resolvedType)) {
                context.reportError(*baseClassRefNode, "Inheritance from primitive types is not allowed");
            } else if (const CustomizedType* baseClassType = dynamic_cast<const CustomizedType*>(resolvedType)) {
                if (baseClassType->getCustomizedKind() != CustomizedTypeKind::CLASS) {
                    context.reportError(*baseClassRefNode, "Inheritance from non-class types is not allowed");
                } else {
                    bool inheritanceFromFinal = false;

                    if (baseClassType->getOrigin() == CustomizedTypeOrigin::LOCAL) {
                        inheritanceFromFinal = dynamic_cast<const ClassDeclarationNode*>(baseClassType->getLocalNode())->isFinal();
                    } else if (baseClassType->getOrigin() == CustomizedTypeOrigin::IMPORTED) {
                        inheritanceFromFinal = dynamic_cast<const ImportedClass*>(baseClassType->getImportedNode())->isFinal();
                    }

                    if (inheritanceFromFinal) {
                        context.reportError(*baseClassRefNode, "Inheritance from final classes is not allowed");
                    }
                }
            }

            const ClassInheritanceNode& inheritanceNode = context.getOrCreateLocalInheritanceNode(classDecl);
            if (inheritanceNode.hasCycle()) {
                context.reportError(*baseClassRefNode, "Cyclic inheritance detected");
            }
        }

        for (const auto& interfaceRefNode : classDecl.getImplementedInterfaces()) {
            const Type* resolvedType = context.getTypeByTypeReferenceNode(interfaceRefNode.get());

            if (dynamic_cast<const PrimitiveType*>(resolvedType)) {
                context.reportError(*interfaceRefNode.get(), fmt::format("Type '{}' is not an interface", resolvedType->getFullTypeName()));
            } else if (const CustomizedType* interfaceType = dynamic_cast<const CustomizedType*>(resolvedType)) {
                if (interfaceType->getCustomizedKind() != CustomizedTypeKind::INTERFACE) {
                    context.reportError(*interfaceRefNode.get(), fmt::format("Type '{}' is not an interface", resolvedType->getFullTypeName()));
                }
            }
        }

        for (const auto& propertyDecl : classDecl.getPropertyDeclarations()) {
            checkValueDeclaration(*propertyDecl);
        }

        for (const auto& methodDecl : classDecl.getMethodDeclarations()) {
            checkFunctionDeclaration(*methodDecl);
        }

        for (const auto& constructorDecl : classDecl.getConstructorDeclarations()) {
            checkConstructorDeclaration(*constructorDecl);
        }

        for (const auto& operatorDecl : classDecl.getOperatorDeclarations()) {
            checkOperatorDeclaration(*operatorDecl);
        }

        context.popScope();
    }

    void SemanticAnalyzer::checkInterfaceDeclaration(const InterfaceDeclarationNode& interfaceDecl, const Config& config) {
        if (!context.pushExistingScope(&interfaceDecl)) {
            return;
        }

        for (const auto& funcDecl : interfaceDecl.getMethodDeclarations()) {
            checkFunctionDeclaration(*funcDecl);
        }

        for (const auto& operatorDecl : interfaceDecl.getOperatorDeclarations()) {
            checkOperatorDeclaration(*operatorDecl);
        }

        context.popScope();
    }

    void SemanticAnalyzer::checkEnumDeclaration(const EnumDeclarationNode& enumDecl, const Config& config) {
        if (!context.pushExistingScope(&enumDecl)) {
            return;
        }

        for (const auto& member : enumDecl.getMemberDeclarations()) {
            if (!context.pushExistingScope(member.get())) {
                continue;
            }

            for (auto& associatedValue : member->getAssociatedValues()) {
                checkValueDeclaration(*associatedValue);
            }

            context.popScope();
        }

        context.popScope();
    }

    void SemanticAnalyzer::checkStatement(const StatementNode& statement) {
        if (auto* stmt = dynamic_cast<const BlockStatementNode*>(&statement)) {
            checkBlockStatement(*stmt);
        } else if (auto* stmt = dynamic_cast<const BreakStatementNode*>(&statement)) {
            checkBreakStatement(*stmt);
        } else if (auto* stmt = dynamic_cast<const ContinueStatementNode*>(&statement)) {
            checkContinueStatement(*stmt);
        } else if (auto* stmt = dynamic_cast<const ExpressionStatementNode*>(&statement)) {
            checkExpressionStatement(*stmt);
        } else if (auto* stmt = dynamic_cast<const ForStatementNode*>(&statement)) {
            checkForStatement(*stmt);
        } else if (auto* stmt = dynamic_cast<const IfStatementNode*>(&statement)) {
            checkIfStatement(*stmt);
        } else if (auto* stmt = dynamic_cast<const ReloadStatementNode*>(&statement)) {
            checkReloadStatement(*stmt);
        } else if (auto* stmt = dynamic_cast<const ReturnStatementNode*>(&statement)) {
            checkReturnStatement(*stmt);
        } else if (auto* stmt = dynamic_cast<const SwitchStatementNode*>(&statement)) {
            checkSwitchStatement(*stmt);
        } else if (auto* stmt = dynamic_cast<const VariableDeclarationStatementNode*>(&statement)) {
            checkVariableDeclarationStatement(*stmt);
        } else if (auto* stmt = dynamic_cast<const WhileStatementNode*>(&statement)) {
            checkWhileStatement(*stmt);
        } else {
            context.reportError(statement, "Unknown statement type");
        }
    }

    void SemanticAnalyzer::checkBlockStatement(const BlockStatementNode& blockStmt) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::BLOCK, &context.currentScope(), &blockStmt));

        for (const auto& child : blockStmt.getStatements()) {
            checkStatement(*child);
        }

        context.popScope();
    }

    void SemanticAnalyzer::checkBreakStatement(const BreakStatementNode& breakStmt) {
        if (!context.currentLoopScope()) {
            context.reportError(breakStmt, "Break statement not within a loop");
        }

        const auto& label = breakStmt.getLabel();
        if (label.has_value()) {
            const auto* labelSymbol = dynamic_cast<const RegularSymbol*>(context.currentScope().lookup(label.value()->getIdentifierString()));
            if (labelSymbol == nullptr) {
                context.reportError(breakStmt, fmt::format("Label '{}' does not exist", label.value()->getIdentifierString()));
            } else if (labelSymbol->getKind() != RegularSymbolKind::LOOP_LABEL) {
                context.reportError(breakStmt, fmt::format("Identifier '{}' is not a loop label", label.value()->getIdentifierString()));
            }
        }
    }

    void SemanticAnalyzer::checkContinueStatement(const ContinueStatementNode& continueStmt) {
        if (!context.currentLoopScope()) {
            context.reportError(continueStmt, "Continue statement not within a loop");
        }

        const auto& label = continueStmt.getLabel();
        if (label.has_value()) {
            const auto* labelSymbol = dynamic_cast<const RegularSymbol*>(context.currentScope().lookup(label.value()->getIdentifierString()));
            if (labelSymbol == nullptr) {
                context.reportError(continueStmt, fmt::format("Label '{}' does not exist", label.value()->getIdentifierString()));
            } else if (labelSymbol->getKind() != RegularSymbolKind::LOOP_LABEL) {
                context.reportError(continueStmt, fmt::format("Identifier '{}' is not a loop label", label.value()->getIdentifierString()));
            }
        }
    }

    void SemanticAnalyzer::checkExpressionStatement(const ExpressionStatementNode& exprStmt) {
        checkExpression(exprStmt.getExpression());
    }

    void SemanticAnalyzer::checkForStatement(const ForStatementNode& forStmt) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::LOOP, &context.currentScope(), &forStmt));

        const auto& label = forStmt.getLabel();
        if (label.has_value()) {
            RegularSymbol labelSymbol(RegularSymbolKind::LOOP_LABEL, RegularSymbolAccessModifier::PUBLIC, label.value()->getIdentifierString(), &forStmt);
            if (!context.currentScope().declare(std::move(labelSymbol))) {
                context.reportError(forStmt, fmt::format("Redeclaration of identifier '{}'", label.value()->getIdentifierString()));
            }
        }

        const auto& loopVariable = forStmt.getLoopVariable();
        RegularSymbol loopVariableSymbol(
            RegularSymbolKind::VARIABLE,
            static_cast<RegularSymbolAccessModifier>(loopVariable.getAccessModifier()),
            loopVariable.getName().getIdentifierString(),
            &loopVariable
        );
        if (!context.currentScope().declare(std::move(loopVariableSymbol))) {
            context.reportError(loopVariable, fmt::format("Redeclaration of symbol '{}'", loopVariable.getName().getIdentifierString()));
        }

        if (loopVariable.getType().has_value()) {
            resolveTypeReference(*loopVariable.getType().value());
        }
        checkValueDeclaration(forStmt.getLoopVariable());
        checkExpression(forStmt.getIterableExpression());
        checkStatement(forStmt.getBody());

        context.popScope();
    }

    void SemanticAnalyzer::checkIfStatement(const IfStatementNode& ifStmt) {
        checkExpression(ifStmt.getCondition());
        checkStatement(ifStmt.getThenBranch());
        if (ifStmt.getElseBranch().has_value()) {
            checkStatement(*ifStmt.getElseBranch().value());
        }
    }

    void SemanticAnalyzer::checkReloadStatement(const ReloadStatementNode& reloadStmt) {}

    void SemanticAnalyzer::checkReturnStatement(const ReturnStatementNode& returnStmt) {
        if (returnStmt.getReturnValue().has_value()) {
            checkExpression(*returnStmt.getReturnValue().value());
        }
    }

    void SemanticAnalyzer::checkSwitchStatement(const SwitchStatementNode& switchStmt) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::SWITCH, &context.currentScope(), &switchStmt));

        checkExpression(switchStmt.getSwitchExpression());

        if (switchStmt.getSwitchKind() == SwitchStatementKind::LITERAL_MATCH) {
            for (const auto& item : switchStmt.getLiteralMatchItems()) {
                checkStatement(*item.body);
            }
        } else if (switchStmt.getSwitchKind() == SwitchStatementKind::TYPE_MATCH) {
            for (const auto& item : switchStmt.getTypeMatchItems()) {
                resolveTypeReference(*item.type);
                checkStatement(*item.body);
            }
        }

        if (switchStmt.getDefaultCaseBody().has_value()) {
            checkStatement(*switchStmt.getDefaultCaseBody().value());
        }

        context.popScope();
    }

    void SemanticAnalyzer::checkVariableDeclarationStatement(const VariableDeclarationStatementNode& varDeclStmt) {
        const auto& varDecl = varDeclStmt.getVariableDeclaration();
        RegularSymbol variableSymbol(RegularSymbolKind::VARIABLE, static_cast<RegularSymbolAccessModifier>(varDecl.getAccessModifier()), varDecl.getName().getIdentifierString(), &varDecl);
        if (!context.currentScope().declare(std::move(variableSymbol))) {
            context.reportError(varDecl, fmt::format("Redeclaration of symbol '{}'", varDecl.getName().getIdentifierString()));
        }
        if (varDecl.getType().has_value()) {
            resolveTypeReference(*varDecl.getType().value());
        }
        checkValueDeclaration(varDecl);
    }

    void SemanticAnalyzer::checkWhileStatement(const WhileStatementNode& whileStmt) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::LOOP, &context.currentScope(), &whileStmt));

        const auto& label = whileStmt.getLabel();
        if (label.has_value()) {
            RegularSymbol labelSymbol(RegularSymbolKind::LOOP_LABEL, RegularSymbolAccessModifier::PUBLIC, label.value()->getIdentifierString(), &whileStmt);
            if (!context.currentScope().declare(std::move(labelSymbol))) {
                context.reportError(whileStmt, fmt::format("Redeclaration of identifier '{}'", label.value()->getIdentifierString()));
            }
        }

        checkExpression(whileStmt.getCondition());
        checkStatement(whileStmt.getBody());

        context.popScope();
    }

    void SemanticAnalyzer::checkExpression(const ExpressionNode& expression) {
        if (auto* expr = dynamic_cast<const BinaryExpressionNode*>(&expression)) {
            checkBinaryExpression(*expr);
        } else if (auto* expr = dynamic_cast<const ConditionalExpressionNode*>(&expression)) {
            checkConditionalExpression(*expr);
        } else if (auto* expr = dynamic_cast<const DictLiteralExpressionNode*>(&expression)) {
            checkDictLiteralExpression(*expr);
        } else if (auto* expr = dynamic_cast<const FunctionCallExpressionNode*>(&expression)) {
            checkFunctionCallExpression(*expr);
        } else if (auto* expr = dynamic_cast<const IdentifierLikeExpressionNode*>(&expression)) {
            checkIdentifierLikeExpression(*expr);
        } else if (auto* expr = dynamic_cast<const PrimitiveTypeExpressionNode*>(&expression)) {
            checkPrimitiveTypeExpression(*expr);
        } else if (auto* expr = dynamic_cast<const ListLikeLiteralExpressionNode*>(&expression)) {
            checkListLikeLiteralExpression(*expr);
        } else if (auto* expr = dynamic_cast<const MemberAccessExpressionNode*>(&expression)) {
            checkMemberAccessExpression(*expr);
        } else if (auto* expr = dynamic_cast<const RangeExpressionNode*>(&expression)) {
            checkRangeExpression(*expr);
        } else if (auto* expr = dynamic_cast<const SelectorLiteralExpressionNode*>(&expression)) {
            checkSelectorLiteralExpression(*expr);
        } else if (auto* expr = dynamic_cast<const SimpleLiteralExpressionNode*>(&expression)) {
            checkSimpleLiteralExpression(*expr);
        } else if (auto* expr = dynamic_cast<const StringLiteralExpressionNode*>(&expression)) {
            checkStringLiteralExpression(*expr);
        } else if (auto* expr = dynamic_cast<const SubscriptExpressionNode*>(&expression)) {
            checkSubscriptExpression(*expr);
        } else if (auto* expr = dynamic_cast<const SuperExpressionNode*>(&expression)) {
            checkSuperExpression(*expr);
        } else if (auto* expr = dynamic_cast<const NoneExpressionNode*>(&expression)) {
            checkNoneExpression(*expr);
        } else if (auto* expr = dynamic_cast<const ThisExpressionNode*>(&expression)) {
            checkThisExpression(*expr);
        } else if (auto* expr = dynamic_cast<const UnaryExpressionNode*>(&expression)) {
            checkUnaryExpression(*expr);
        } else {
            context.reportError(expression, "Unknown expression type");
        }
    }

    void SemanticAnalyzer::checkBinaryExpression(const BinaryExpressionNode& binaryExpr) {
        // TODO: Implement binary expression checking
        checkExpression(binaryExpr.getLeft());
        checkExpression(binaryExpr.getRight());
    }

    void SemanticAnalyzer::checkConditionalExpression(const ConditionalExpressionNode& conditionalExpr) {
        // TODO: Implement conditional expression checking
        checkExpression(conditionalExpr.getCondition());
        checkExpression(conditionalExpr.getThenExpression());
        checkExpression(conditionalExpr.getElseExpression());
    }

    void SemanticAnalyzer::checkDictLiteralExpression(const DictLiteralExpressionNode& dictLiteralExpr) {
        // TODO: Implement dictionary literal expression checking
        for (const auto& entry : dictLiteralExpr.getEntries()) {
            checkExpression(*entry.second);
        }
    }

    void SemanticAnalyzer::checkFunctionCallExpression(const FunctionCallExpressionNode& funcCallExpr) {
        // TODO: Implement function call expression checking
        checkExpression(funcCallExpr.getCallee());
        for (const auto& argument : funcCallExpr.getArguments()) {
            checkExpression(*argument);
        }
        if (funcCallExpr.getContext().has_value()) {
            checkExpression(*funcCallExpr.getContext().value());
        }
    }

    void SemanticAnalyzer::checkIdentifierLikeExpression(const IdentifierLikeExpressionNode& identifierLikeExpr) {
        // TODO: Implement identifier expression checking
    }

    void SemanticAnalyzer::checkPrimitiveTypeExpression(const PrimitiveTypeExpressionNode& primitiveTypeExpr) {}

    void SemanticAnalyzer::checkListLikeLiteralExpression(const ListLikeLiteralExpressionNode& listLikeLiteralExpr) {
        // TODO: Implement list-like literal expression checking
        for (const auto& element : listLikeLiteralExpr.getElements()) {
            checkExpression(*element);
        }
    }

    void SemanticAnalyzer::checkMemberAccessExpression(const MemberAccessExpressionNode& memberAccessExpr) {
        // TODO: Implement member access expression checking
        checkExpression(memberAccessExpr.getObject());
        checkExpression(memberAccessExpr.getMember());
    }

    void SemanticAnalyzer::checkRangeExpression(const RangeExpressionNode& rangeExpr) {
        // TODO: Implement range expression checking
        if (rangeExpr.getStart().has_value()) {
            checkExpression(*rangeExpr.getStart().value());
        }
        if (rangeExpr.getEnd().has_value()) {
            checkExpression(*rangeExpr.getEnd().value());
        }
    }

    void SemanticAnalyzer::checkSelectorLiteralExpression(const SelectorLiteralExpressionNode& selectorLiteralExpr) {
        // TODO: Implement selector literal expression checking
        for (const auto& argument : selectorLiteralExpr.getArguments()) {
            checkExpression(*argument.second);
        }
    }

    void SemanticAnalyzer::checkSimpleLiteralExpression(const SimpleLiteralExpressionNode& simpleLiteralExpr) {
        // TODO: Implement simple literal expression checking
    }

    void SemanticAnalyzer::checkStringLiteralExpression(const StringLiteralExpressionNode& stringLiteralExpr) {
        // TODO: Implement string literal expression checking
        for (const auto& part : stringLiteralExpr.getParts()) {
            if (const auto* nestedExpression = std::get_if<std::unique_ptr<ExpressionNode>>(&part)) {
                checkExpression(**nestedExpression);
            }
        }
    }

    void SemanticAnalyzer::checkSubscriptExpression(const SubscriptExpressionNode& subscriptExpr) {
        // TODO: Implement subscript expression checking
        checkExpression(subscriptExpr.getObject());
        checkExpression(subscriptExpr.getIndex());
    }

    void SemanticAnalyzer::checkSuperExpression(const SuperExpressionNode& superExpr) {
        // TODO: Implement super expression checking
    }

    void SemanticAnalyzer::checkNoneExpression(const NoneExpressionNode& noneExpr) {
        // TODO: Implement none expression checking
    }

    void SemanticAnalyzer::checkThisExpression(const ThisExpressionNode& thisExpr) {
        // TODO: Implement this expression checking
    }

    void SemanticAnalyzer::checkUnaryExpression(const UnaryExpressionNode& unaryExpr) {
        // TODO: Implement unary expression checking
        checkExpression(unaryExpr.getOperand());
    }

    SemanticResult SemanticAnalyzer::analyze(const Config& config) {
        checkModule(context.getModule(), config);

        auto diagnostics = context.takeDiagnostics();
        auto customizedTypes = context.takeCustomizedTypeRegistry();
        auto scopes = context.takeLocalScopeMap();
        auto importedScopes = context.takeImportedScopeMap();
        auto types = context.takeTypeMap();
        auto inferredValueTypes = context.takeInferredValueTypeMap();
        auto inferredFunctionReturnTypes = context.takeInferredFunctionReturnTypeMap();
        auto inferredOperatorReturnTypes = context.takeInferredOperatorReturnTypeMap();
        auto expressionAnalysisResults = context.takeExpressionAnalysisResultMap();
        auto inheritanceNodes = context.takeInheritanceNodes();
        auto localInheritances = context.takeLocalInheritanceMap();
        auto importedInheritances = context.takeImportedInheritanceMap();
        return {
            std::move(std::get<0>(diagnostics)),
            std::move(std::get<1>(diagnostics)),
            std::move(std::get<2>(diagnostics)),
            std::move(customizedTypes),
            std::move(scopes),
            std::move(importedScopes),
            std::move(types),
            std::move(inferredValueTypes),
            std::move(inferredFunctionReturnTypes),
            std::move(inferredOperatorReturnTypes),
            std::move(expressionAnalysisResults),
            std::move(inheritanceNodes),
            std::move(localInheritances),
            std::move(importedInheritances),
        };
    }
} // namespace vnlc
