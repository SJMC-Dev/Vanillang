#include "ModuleInterfaceFileGenerator.hpp"
#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/declaration/ConstructorDeclarationNode.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/OperatorDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "nlohmann/json_fwd.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <string_view>

namespace vnlc {
    namespace {
        template <typename AccessModifier>
        std::string_view getAccessModifierString(AccessModifier accessModifier) {
            switch (accessModifier) {
                case AccessModifier::PUBLIC:
                    return "public";
                case AccessModifier::PROTECTED:
                    return "protected";
                case AccessModifier::PRIVATE:
                    return "private";
            }

            return "public";
        }
    } // namespace

    ModuleInterfaceFileGenerator::ModuleInterfaceFileGenerator(std::vector<const DeclarationNode*>&& declarationNodes, const Config& config, const SemanticResult& semantic) noexcept
        : declarationNodes(std::move(declarationNodes)),
          config(config),
          semantic(semantic) {}

    nlohmann::json ModuleInterfaceFileGenerator::stringifyMetadata(const std::vector<DeclarationItem::MetadataTerm>& metadataTerms) {
        nlohmann::json metadataObj = nlohmann::json::object();

        for (const auto& metadataTerm : metadataTerms) {
            metadataObj.emplace(metadataTerm.key->getIdentifierString(), metadataTerm.value);
        }

        return metadataObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyGenericParameters(const std::vector<std::unique_ptr<IdentifierNode>>& genericParameterNames) {
        nlohmann::json genericParametersObj = nlohmann::json::array();

        for (const auto& genericParam : genericParameterNames) {
            genericParametersObj.push_back(genericParam->getIdentifierString());
        }

        return genericParametersObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyParameter(const ValueDeclarationNode* parameter) {
        nlohmann::json parameterObj = nlohmann::json::object();
        parameterObj.emplace("category", "parameter");

        const auto* typeNode = parameter->getType();
        if (typeNode != nullptr) {
            const auto* type = semantic.getTypeByTypeAnnotationNode(typeNode);
            if (type != nullptr) {
                parameterObj.emplace("type", type->getFullTypeName());
            }
        } else {
            const auto* inferredType = semantic.getInferredValueType(parameter);
            if (inferredType != nullptr) {
                parameterObj.emplace("type", inferredType->getFullTypeName());
            }
        }

        return parameterObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyVariable(const ValueDeclarationNode* variable) {
        nlohmann::json variableObj = nlohmann::json::object();
        variableObj.emplace("category", "let");

        const auto& metadata = variable->getMetadataTerms();
        if (!metadata.empty()) {
            variableObj.emplace("metadata", stringifyMetadata(metadata));
        }

        const auto* typeNode = variable->getType();
        if (typeNode != nullptr) {
            const auto* type = semantic.getTypeByTypeAnnotationNode(typeNode);
            if (type != nullptr) {
                variableObj.emplace("type", type->getFullTypeName());
            }
        } else {
            const auto* inferredType = semantic.getInferredValueType(variable);
            if (inferredType != nullptr) {
                variableObj.emplace("type", inferredType->getFullTypeName());
            }
        }

        variableObj.emplace("accessModifier", getAccessModifierString(variable->getAccessModifier()));

        return variableObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyFunction(const FunctionDeclarationNode* function) {
        nlohmann::json functionObj = nlohmann::json::object();
        functionObj.emplace("category", "func");
        functionObj.emplace("name", function->getName().getIdentifierString());

        const auto& metadata = function->getMetadataTerms();

        if (!metadata.empty()) {
            functionObj.emplace("metadata", stringifyMetadata(metadata));
        }

        const auto* returnTypeAnnotationNode = function->getReturnType();
        if (returnTypeAnnotationNode != nullptr) {
            const auto* type = semantic.getTypeByTypeAnnotationNode(returnTypeAnnotationNode);
            if (type != nullptr) {
                functionObj.emplace("returnType", type->getFullTypeName());
            }
        } else {
            const auto* inferredReturnType = semantic.getInferredFunctionReturnType(function);
            if (inferredReturnType != nullptr) {
                functionObj.emplace("returnType", inferredReturnType->getFullTypeName());
            }
        }

        nlohmann::json parametersObj = nlohmann::json::object();
        for (const auto& parameter : function->getParameters()) {
            parametersObj.emplace(parameter->getName().getIdentifierString(), stringifyParameter(parameter.get()));
        }

        functionObj.emplace("parameters", parametersObj);
        functionObj.emplace("native", function->getImplementation() == FunctionDeclarationKind::Implementation::NATIVE);
        functionObj.emplace("accessModifier", getAccessModifierString(function->getAccessModifier()));

        return functionObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyClass(const ClassDeclarationNode* classNode) {
        nlohmann::json classObj = nlohmann::json::object();
        classObj.emplace("category", "class");

        const auto& metadata = classNode->getMetadataTerms();
        if (!metadata.empty()) {
            classObj.emplace("metadata", stringifyMetadata(metadata));
        }

        std::optional<std::string> baseClassName;
        if (classNode->getBaseClass() != nullptr) {
            const auto* baseClassTypeAnnotationNode = classNode->getBaseClass();
            const auto* type = semantic.getTypeByTypeAnnotationNode(baseClassTypeAnnotationNode);
            if (type != nullptr) {
                baseClassName = type->getFullTypeName();
            }
        }
        classObj.emplace("baseClass", baseClassName);

        std::vector<std::string> implementedInterfaceNames;
        for (const auto& interfaceTypeAnnotationNode : classNode->getImplementedInterfaces()) {
            const auto* type = semantic.getTypeByTypeAnnotationNode(interfaceTypeAnnotationNode.get());
            if (type != nullptr) {
                implementedInterfaceNames.emplace_back(type->getFullTypeName());
            }
        }
        classObj.emplace("implementedInterfaces", implementedInterfaceNames);

        classObj.emplace("final", classNode->isFinal());

        classObj.emplace("genericParameters", stringifyGenericParameters(classNode->getGenericParameterNames()));

        nlohmann::json propertiesObj = nlohmann::json::object();
        nlohmann::json methodsObj = nlohmann::json::object();
        nlohmann::json constructorsObj = nlohmann::json::object();
        nlohmann::json operatorsObj = nlohmann::json::object();

        for (const auto& property : classNode->getPropertyDeclarations()) {
            propertiesObj.emplace(property->getName().getIdentifierString(), stringifyProperty(property.get()));
        }

        for (const auto& method : classNode->getMethodDeclarations()) {
            methodsObj.emplace(method->getInternalName(), stringifyMethod(method.get()));
        }

        for (const auto& constructor : classNode->getConstructorDeclarations()) {
            constructorsObj.emplace(constructor->getInternalName(), stringifyConstructor(constructor.get()));
        }

        for (const auto& operatorNode : classNode->getOperatorDeclarations()) {
            operatorsObj.emplace(operatorNode->getInternalName(), stringifyOperator(operatorNode.get()));
        }

        classObj.emplace("properties", propertiesObj);
        classObj.emplace("methods", methodsObj);
        classObj.emplace("constructors", constructorsObj);
        classObj.emplace("operators", operatorsObj);
        classObj.emplace("accessModifier", getAccessModifierString(classNode->getAccessModifier()));

        return classObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyInterface(const InterfaceDeclarationNode* interfaceNode) {
        nlohmann::json interfaceObj = nlohmann::json::object();
        interfaceObj.emplace("category", "interface");

        const auto& metadata = interfaceNode->getMetadataTerms();
        if (!metadata.empty()) {
            interfaceObj.emplace("metadata", stringifyMetadata(metadata));
        }

        interfaceObj.emplace("genericParameters", stringifyGenericParameters(interfaceNode->getGenericParameterNames()));

        nlohmann::json methodsObj = nlohmann::json::object();
        for (const auto& method : interfaceNode->getMethodDeclarations()) {
            nlohmann::json methodObj = stringifyMethod(method.get());
            methodsObj.emplace(method->getInternalName(), methodObj);
        }
        interfaceObj.emplace("methods", methodsObj);

        nlohmann::json operatorsObj = nlohmann::json::object();
        for (const auto& operatorNode : interfaceNode->getOperatorDeclarations()) {
            nlohmann::json operatorObj = stringifyOperator(operatorNode.get());
            operatorsObj.emplace(operatorNode->getInternalName(), operatorObj);
        }
        interfaceObj.emplace("operators", operatorsObj);
        interfaceObj.emplace("accessModifier", getAccessModifierString(interfaceNode->getAccessModifier()));

        return interfaceObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyEnum(const EnumDeclarationNode* enumNode) {
        nlohmann::json enumObj = nlohmann::json::object();
        enumObj.emplace("category", "enum");

        enumObj.emplace("genericParameters", stringifyGenericParameters(enumNode->getGenericParameterNames()));

        nlohmann::json membersObj = nlohmann::json::object();
        for (const auto& member : enumNode->getMemberDeclarations()) {
            nlohmann::json memberObj = stringifyEnumMember(member.get());
            membersObj.emplace(member->getName().getIdentifierString(), memberObj);
        }
        enumObj.emplace("members", membersObj);
        enumObj.emplace("accessModifier", getAccessModifierString(enumNode->getAccessModifier()));

        return enumObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyEnumMember(const EnumMemberDeclarationNode* enumMemberNode) {
        nlohmann::json memberObj = nlohmann::json::object();
        memberObj.emplace("category", "enummember");

        nlohmann::json associatedValuesObj = nlohmann::json::object();
        for (const auto& associatedValue : enumMemberNode->getAssociatedValues()) {
            nlohmann::json associatedValueObj = stringifyEnumValue(associatedValue.get());
            associatedValuesObj.emplace(associatedValue->getName().getIdentifierString(), associatedValueObj);
        }
        memberObj.emplace("associatedValues", associatedValuesObj);

        return memberObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyEnumValue(const ValueDeclarationNode* enumValue) {
        nlohmann::json enumValueObj = nlohmann::json::object();
        enumValueObj.emplace("category", "enumvalue");

        const auto* typeNode = enumValue->getType();
        if (typeNode != nullptr) {
            const auto* type = semantic.getTypeByTypeAnnotationNode(typeNode);
            if (type != nullptr) {
                enumValueObj.emplace("type", type->getFullTypeName());
            }
        } else {
            const auto* inferredType = semantic.getInferredValueType(enumValue);
            if (inferredType != nullptr) {
                enumValueObj.emplace("type", inferredType->getFullTypeName());
            }
        }

        return enumValueObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyTypeAlias(const TypeAliasDeclarationNode* typeAliasNode) {
        nlohmann::json typeAliasObj = nlohmann::json::object();
        typeAliasObj.emplace("category", "typealias");

        typeAliasObj.emplace("genericParameters", stringifyGenericParameters(typeAliasNode->getGenericParameterNames()));
        const auto* type = semantic.getTypeByTypeAnnotationNode(&typeAliasNode->getOriginalType());
        if (type != nullptr) {
            typeAliasObj.emplace("originalType", type->getFullTypeName());
        }

        typeAliasObj.emplace("accessModifier", getAccessModifierString(typeAliasNode->getAccessModifier()));

        return typeAliasObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyProperty(const ValueDeclarationNode* property) {
        nlohmann::json propertyObj = nlohmann::json::object();
        propertyObj.emplace("category", "property");

        const auto& metadata = property->getMetadataTerms();
        if (!metadata.empty()) {
            propertyObj.emplace("metadata", stringifyMetadata(metadata));
        }

        const auto* typeNode = property->getType();
        if (typeNode != nullptr) {
            const auto* type = semantic.getTypeByTypeAnnotationNode(typeNode);
            if (type != nullptr) {
                propertyObj.emplace("type", type->getFullTypeName());
            }
        } else {
            const auto* inferredType = semantic.getInferredValueType(property);
            if (inferredType != nullptr) {
                propertyObj.emplace("type", inferredType->getFullTypeName());
            }
        }

        propertyObj.emplace("static", property->getKind() == ValueDeclarationKind::Kind::STATIC_PROPERTY);
        propertyObj.emplace("accessModifier", getAccessModifierString(property->getAccessModifier()));

        return propertyObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyMethod(const FunctionDeclarationNode* method) {
        nlohmann::json methodObj = nlohmann::json::object();
        methodObj.emplace("category", "method");
        methodObj.emplace("name", method->getName().getIdentifierString());

        const auto& metadata = method->getMetadataTerms();

        if (!metadata.empty()) {
            methodObj.emplace("metadata", stringifyMetadata(metadata));
        }

        const auto* returnTypeAnnotationNode = method->getReturnType();
        if (returnTypeAnnotationNode != nullptr) {
            const auto* type = semantic.getTypeByTypeAnnotationNode(returnTypeAnnotationNode);
            if (type != nullptr) {
                methodObj.emplace("returnType", type->getFullTypeName());
            }
        } else {
            const auto* inferredReturnType = semantic.getInferredFunctionReturnType(method);
            if (inferredReturnType != nullptr) {
                methodObj.emplace("returnType", inferredReturnType->getFullTypeName());
            }
        }

        nlohmann::json parametersObj = nlohmann::json::object();
        for (const auto& parameter : method->getParameters()) {
            parametersObj.emplace(parameter->getName().getIdentifierString(), stringifyParameter(parameter.get()));
        }

        methodObj.emplace("parameters", parametersObj);
        methodObj.emplace("native", method->getImplementation() == FunctionDeclarationKind::Implementation::NATIVE);
        methodObj.emplace("static", method->getBinding() == FunctionDeclarationKind::Binding::STATIC);
        methodObj.emplace("accessModifier", getAccessModifierString(method->getAccessModifier()));

        return methodObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyConstructor(const ConstructorDeclarationNode* constructor) {
        nlohmann::json constructorObj = nlohmann::json::object();
        constructorObj.emplace("category", "constructor");

        const auto& metadata = constructor->getMetadataTerms();
        if (!metadata.empty()) {
            constructorObj.emplace("metadata", stringifyMetadata(metadata));
        }

        nlohmann::json parametersObj = nlohmann::json::object();
        for (const auto& parameter : constructor->getParameters()) {
            parametersObj.emplace(parameter->getName().getIdentifierString(), stringifyParameter(parameter.get()));
        }

        constructorObj.emplace("parameters", parametersObj);
        constructorObj.emplace("accessModifier", getAccessModifierString(constructor->getAccessModifier()));

        return constructorObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyOperator(const OperatorDeclarationNode* operatorNode) {
        nlohmann::json operatorObj = nlohmann::json::object();
        operatorObj.emplace("category", "operator");

        const auto& metadata = operatorNode->getMetadataTerms();
        if (!metadata.empty()) {
            operatorObj.emplace("metadata", stringifyMetadata(metadata));
        }

        const auto* returnTypeAnnotationNode = operatorNode->getReturnType();
        if (returnTypeAnnotationNode != nullptr) {
            const auto* type = semantic.getTypeByTypeAnnotationNode(returnTypeAnnotationNode);
            if (type != nullptr) {
                operatorObj.emplace("returnType", type->getFullTypeName());
            }
        } else {
            const auto* inferredReturnType = semantic.getInferredOperatorReturnType(operatorNode);
            if (inferredReturnType != nullptr) {
                operatorObj.emplace("returnType", inferredReturnType->getFullTypeName());
            }
        }

        nlohmann::json parametersObj = nlohmann::json::object();
        for (const auto& parameter : operatorNode->getParameters()) {
            parametersObj.emplace(parameter->getName().getIdentifierString(), stringifyParameter(parameter.get()));
        }
        operatorObj.emplace("parameters", parametersObj);
        operatorObj.emplace("native", operatorNode->getImplementation() == OperatorDeclarationKind::Implementation::NATIVE);
        operatorObj.emplace("accessModifier", getAccessModifierString(operatorNode->getAccessModifier()));

        return operatorObj;
    }

    void ModuleInterfaceFileGenerator::generate() {
        if (config.moduleInterfaceOutputDirectory.empty()) {
            return;
        }

        nlohmann::json moduleObj = nlohmann::json::object();

        for (const auto* declarationNode : declarationNodes) {
            if (const auto* variable = dynamic_cast<const ValueDeclarationNode*>(declarationNode)) {
                moduleObj.emplace(variable->getName().getIdentifierString(), stringifyVariable(variable));
            } else if (const auto* function = dynamic_cast<const FunctionDeclarationNode*>(declarationNode)) {
                moduleObj.emplace(function->getInternalName(), stringifyFunction(function));
            } else if (const auto* classNode = dynamic_cast<const ClassDeclarationNode*>(declarationNode)) {
                moduleObj.emplace(classNode->getName().getIdentifierString(), stringifyClass(classNode));
            } else if (const auto* interfaceNode = dynamic_cast<const InterfaceDeclarationNode*>(declarationNode)) {
                moduleObj.emplace(interfaceNode->getName().getIdentifierString(), stringifyInterface(interfaceNode));
            } else if (const auto* enumNode = dynamic_cast<const EnumDeclarationNode*>(declarationNode)) {
                moduleObj.emplace(enumNode->getName().getIdentifierString(), stringifyEnum(enumNode));
            } else if (const auto* typeAliasNode = dynamic_cast<const TypeAliasDeclarationNode*>(declarationNode)) {
                moduleObj.emplace(typeAliasNode->getAliasName().getIdentifierString(), stringifyTypeAlias(typeAliasNode));
            } else {
                assert(false && "Unsupported declaration node in module interface file generator");
            }
        }

        std::filesystem::path relativeInputPath = std::filesystem::relative(config.inputFilePath, config.packageRootPath);
        std::filesystem::path outputPath = config.moduleInterfaceOutputDirectory / config.packageRootPath.filename() / relativeInputPath;
        outputPath.replace_extension(".vni");

        std::filesystem::create_directories(outputPath.parent_path());

        std::ofstream outputFile;
        outputFile.exceptions(std::ios::failbit | std::ios::badbit);
        outputFile.open(outputPath);
        outputFile << moduleObj.dump(4) << '\n';
    }
} // namespace vnlc
