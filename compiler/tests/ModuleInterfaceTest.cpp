#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/EnumDeclarationNode.hpp"
#include "ast/declaration/EnumMemberDeclarationNode.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include "ast/declaration/OperatorDeclarationNode.hpp"
#include "ast/declaration/TypeAliasDeclarationNode.hpp"
#include "ast/declaration/TypeDeclarationKind.hpp"
#include "ast/declaration/ValueDeclarationKind.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/identifier/IdentifierNode.hpp"
#include "ast/statement/BlockStatementNode.hpp"
#include "ast/typeannotation/CustomizedTypeAnnotationNode.hpp"
#include "ast/typeannotation/TypeAnnotationNode.hpp"
#include "config/Config.hpp"
#include "config/RunningMode.hpp"
#include "error/ModuleInterfaceReaderError.hpp"
#include "error/PackageReaderError.hpp"
#include "token/Token.hpp"
#include "type/PrimitiveType.hpp"
#include "vni/export/ModuleInterfaceFileGenerator.hpp"
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
#include "vni/import/ModuleInterfaceFileReader.hpp"
#include "vni/import/PackageReader.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace vnlc {

    namespace {

        const Token testToken(TokenKind::IDENTIFIER, "test", 1, 1, 0);

        using TypeMap = std::unordered_map<const TypeAnnotationNode*, const Type*>;
        using InferredValueTypeMap = std::unordered_map<const ValueDeclarationNode*, const Type*>;
        using InferredFunctionReturnTypeMap = std::unordered_map<const FunctionDeclarationNode*, const Type*>;
        using InferredOperatorReturnTypeMap = std::unordered_map<const OperatorDeclarationNode*, const Type*>;

        std::unique_ptr<IdentifierNode> makeIdentifier(std::string_view name) {
            return std::make_unique<IdentifierNode>(name, testToken, testToken);
        }

        std::unique_ptr<TypeAnnotationNode> makeType(std::string_view name) {
            std::vector<std::unique_ptr<IdentifierNode>> nameParts;
            nameParts.push_back(makeIdentifier(name));
            return std::make_unique<CustomizedTypeAnnotationNode>(false, std::move(nameParts), std::vector<std::unique_ptr<TypeAnnotationNode>>{}, testToken, testToken);
        }

        std::vector<DeclarationItem::MetadataTerm> makeMetadata() {
            std::vector<DeclarationItem::MetadataTerm> metadata;
            metadata.push_back({ makeIdentifier("since"), std::string("1.0") });
            metadata.push_back({ makeIdentifier("draft"), std::nullopt });
            return metadata;
        }

        std::unique_ptr<ValueDeclarationNode> makeValue(
            ValueDeclarationKind::Kind kind,
            ValueDeclarationKind::Context context,
            ValueDeclarationKind::AccessModifier accessModifier,
            std::string_view name,
            std::unique_ptr<TypeAnnotationNode>&& type = nullptr
        ) {
            return std::make_unique<ValueDeclarationNode>(kind, context, accessModifier, makeIdentifier(name), std::move(type), nullptr, testToken, testToken);
        }

        std::unique_ptr<ValueDeclarationNode> makeValueWithMetadata(
            ValueDeclarationKind::Kind kind,
            ValueDeclarationKind::Context context,
            ValueDeclarationKind::AccessModifier accessModifier,
            std::string_view name,
            std::unique_ptr<TypeAnnotationNode>&& type,
            std::vector<DeclarationItem::MetadataTerm>&& metadata
        ) {
            return std::make_unique<ValueDeclarationNode>(kind, context, accessModifier, makeIdentifier(name), std::move(type), nullptr, testToken, testToken, std::move(metadata));
        }

        std::unique_ptr<EnumDeclarationNode> makeEnum(
            TypeDeclarationKind::AccessModifier accessModifier,
            std::string_view name,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
            std::vector<std::unique_ptr<EnumMemberDeclarationNode>>&& memberDeclarations
        ) {
            return std::make_unique<EnumDeclarationNode>(accessModifier, makeIdentifier(name), std::move(genericParameters), std::move(memberDeclarations), testToken, testToken);
        }

        std::unique_ptr<TypeAliasDeclarationNode> makeTypeAlias(
            TypeDeclarationKind::AccessModifier accessModifier,
            std::string_view name,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
            std::unique_ptr<TypeAnnotationNode>&& originalType
        ) {
            return std::make_unique<TypeAliasDeclarationNode>(accessModifier, makeIdentifier(name), std::move(genericParameters), std::move(originalType), testToken, testToken);
        }

        std::unique_ptr<FunctionDeclarationNode> makeFunction(FunctionDeclarationKind::AccessModifier accessModifier, std::string_view name) {
            auto function = std::make_unique<FunctionDeclarationNode>(
                FunctionDeclarationKind::Implementation::NATIVE,
                FunctionDeclarationKind::Context::TOP_LEVEL,
                accessModifier,
                FunctionDeclarationKind::Binding::STATIC,
                makeIdentifier(name),
                std::vector<std::unique_ptr<ValueDeclarationNode>>{},
                nullptr,
                nullptr,
                testToken,
                testToken
            );
            function->setInternalName(name);
            return function;
        }

        std::unique_ptr<ClassDeclarationNode> makeClass(TypeDeclarationKind::AccessModifier accessModifier, std::string_view name) {
            return std::make_unique<ClassDeclarationNode>(
                accessModifier,
                false,
                makeIdentifier(name),
                nullptr,
                std::vector<std::unique_ptr<TypeAnnotationNode>>{},
                std::vector<std::unique_ptr<IdentifierNode>>{},
                std::vector<std::unique_ptr<ValueDeclarationNode>>{},
                std::vector<std::unique_ptr<FunctionDeclarationNode>>{},
                std::vector<std::unique_ptr<ConstructorDeclarationNode>>{},
                std::vector<std::unique_ptr<OperatorDeclarationNode>>{},
                testToken,
                testToken
            );
        }

        std::unique_ptr<InterfaceDeclarationNode> makeInterface(TypeDeclarationKind::AccessModifier accessModifier, std::string_view name) {
            return std::make_unique<InterfaceDeclarationNode>(
                accessModifier,
                makeIdentifier(name),
                std::vector<std::unique_ptr<IdentifierNode>>{},
                std::vector<std::unique_ptr<FunctionDeclarationNode>>{},
                std::vector<std::unique_ptr<OperatorDeclarationNode>>{},
                testToken,
                testToken
            );
        }

        SemanticResult makeSemanticResult(
            TypeMap&& typeMap = {},
            InferredValueTypeMap&& inferredValueTypeMap = {},
            InferredFunctionReturnTypeMap&& inferredFunctionReturnTypeMap = {},
            InferredOperatorReturnTypeMap&& inferredOperatorReturnTypeMap = {}
        ) {
            return SemanticResult(
                {},
                {},
                {},
                {},
                {},
                {},
                std::move(typeMap),
                std::move(inferredValueTypeMap),
                std::move(inferredFunctionReturnTypeMap),
                std::move(inferredOperatorReturnTypeMap),
                {},
                {},
                {},
                {}
            );
        }

        Config makeGeneratorConfig(const std::filesystem::path& testDirectory) {
            const auto packageRoot = testDirectory / "package";
            const auto inputFilePath = packageRoot / "models" / "generated.vnl";
            std::filesystem::create_directories(inputFilePath.parent_path());

            return Config{
                .mode = RunningMode::COMPILE,
                .vanillangVersion = "1.0",
                .minecraftVersion = "26.1.2",
                .packageRootPath = packageRoot,
                .inputFilePath = inputFilePath,
                .outputDirectory = {},
                .moduleInterfaceOutputDirectory = testDirectory / "interfaces",
                .dependencyPackageRootPaths = {},
                .optimizationLevel = std::nullopt,
            };
        }

        std::filesystem::path generatedOutputPath(const Config& config) {
            const auto relativeInputPath = std::filesystem::relative(config.inputFilePath, config.packageRootPath);
            auto outputPath = config.moduleInterfaceOutputDirectory / config.packageRootPath.filename() / relativeInputPath;
            outputPath.replace_extension(".vni");
            return outputPath;
        }

        nlohmann::json readJson(const std::filesystem::path& path) {
            std::ifstream input(path);
            return nlohmann::json::parse(input);
        }

        ImportedRootPackageMap makeNestedImports() {
            std::unordered_map<std::string, std::unique_ptr<ImportedIdentifier>> identifiers;
            identifiers.emplace(
                "State",
                std::make_unique<ImportedEnum>("State", std::vector<std::string>{ "T" }, std::unordered_map<std::string, std::unique_ptr<ImportedEnumMember>>{}, "public")
            );

            std::unordered_map<std::string, std::unique_ptr<ImportedModule>> modules;
            modules.emplace("api", std::make_unique<ImportedModule>("api", std::move(identifiers)));

            ImportedRootPackageMap subPackages;
            subPackages.emplace("sub", std::make_unique<ImportedPackage>("sub", ImportedRootPackageMap{}, std::move(modules)));

            ImportedRootPackageMap imports;
            imports.emplace("root", std::make_unique<ImportedPackage>("root", std::move(subPackages), std::unordered_map<std::string, std::unique_ptr<ImportedModule>>{}));
            return imports;
        }

        class VniTest : public testing::Test {
        protected:
            std::filesystem::path testDirectory;

            void SetUp() override {
                const auto* testInfo = testing::UnitTest::GetInstance()->current_test_info();
                std::string directoryName = "vnlctest-vni-" + std::string(testInfo->test_suite_name()) + "-" + std::string(testInfo->name());
                for (auto& character : directoryName) {
                    if (character == '/') {
                        character = '_';
                    }
                }

                testDirectory = std::filesystem::temp_directory_path() / directoryName;
                std::filesystem::remove_all(testDirectory);
                std::filesystem::create_directories(testDirectory);
            }

            void TearDown() override {
                std::error_code error;
                std::filesystem::remove_all(testDirectory, error);
            }

            [[nodiscard]] std::filesystem::path writeFile(const std::filesystem::path& relativePath, std::string_view contents) const {
                const auto path = testDirectory / relativePath;
                std::filesystem::create_directories(path.parent_path());
                std::ofstream output(path);
                output << contents;
                return path;
            }
        };

        class ImportedLetMetadataProbe : public ImportedLet {
        public:
            using ImportedLet::ImportedLet;

            [[nodiscard]] const std::unordered_map<std::string, std::optional<std::string>>& getMetadataProbe() const {
                return getMetadata();
            }
        };

    } // namespace

    TEST_F(VniTest, ImportedItemExposesName) {
        const ImportedLet importedLet("value", "int", "public");

        EXPECT_EQ(importedLet.getName(), "value");
    }

    TEST_F(VniTest, ImportedLetStoresNameAndType) {
        const ImportedLet importedLet("value", "string", "public");

        EXPECT_EQ(importedLet.getName(), "value");
        EXPECT_EQ(importedLet.getType(), "string");
    }

    TEST_F(VniTest, ImportedLetStoresMetadata) {
        std::unordered_map<std::string, std::optional<std::string>> metadata;
        metadata.emplace("since", "1.0");
        metadata.emplace("draft", std::nullopt);

        const ImportedLetMetadataProbe importedLet("value", "int", "public", std::move(metadata));
        const auto& storedMetadata = importedLet.getMetadataProbe();

        ASSERT_EQ(storedMetadata.size(), 2);
        ASSERT_TRUE(storedMetadata.at("since").has_value());
        EXPECT_EQ(storedMetadata.at("since").value(), "1.0");
        EXPECT_FALSE(storedMetadata.at("draft").has_value());
    }

    TEST_F(VniTest, ImportedTopLevelIdentifiersStoreAccessModifiers) {
        const ImportedLet hiddenLet("hidden", "int", "private");
        const ImportedFunc hiddenFunction("hidden", "hidden", "void", {}, false, "private");
        const ImportedClass hiddenClass("Hidden", std::nullopt, {}, false, {}, {}, {}, {}, {}, "private");
        const ImportedInterface hiddenInterface("Hidden", {}, {}, {}, "private");
        const ImportedEnum hiddenEnum("Hidden", {}, {}, "private");
        const ImportedTypeAlias hiddenTypeAlias("Hidden", {}, "int", "private");
        const ImportedLet visibleLet("visible", "int", "public");

        for (const ImportedIdentifier* identifier :
             std::initializer_list<const ImportedIdentifier*>{ &hiddenLet, &hiddenFunction, &hiddenClass, &hiddenInterface, &hiddenEnum, &hiddenTypeAlias }) {
            EXPECT_EQ(identifier->getAccessModifier(), "private");
        }
        EXPECT_EQ(visibleLet.getAccessModifier(), "public");
    }

    TEST_F(VniTest, ImportedIdentifiersExposeAccessModifiersThroughTheirBaseClass) {
        const ImportedProperty property("count", "int", false, "private");
        const ImportedMethod method("run", "run", "void", {}, false, false, "protected");
        const ImportedParameter parameter("amount", "int");
        const ImportedEnumValue enumValue("code", "int");
        const ImportedEnumMember enumMember("Ready", {});

        EXPECT_EQ(static_cast<const ImportedIdentifier&>(property).getAccessModifier(), "private");
        EXPECT_EQ(static_cast<const ImportedIdentifier&>(method).getAccessModifier(), "protected");
        for (const ImportedIdentifier* identifier : std::initializer_list<const ImportedIdentifier*>{ &parameter, &enumValue, &enumMember }) {
            EXPECT_EQ(identifier->getAccessModifier(), "public");
        }
    }

    TEST_F(VniTest, ImportedParameterStoresNameAndType) {
        const ImportedParameter parameter("amount", "int");

        EXPECT_EQ(parameter.getName(), "amount");
        EXPECT_EQ(parameter.getType(), "int");
    }

    TEST_F(VniTest, ImportedEnumValueStoresNameAndType) {
        const ImportedEnumValue enumValue("code", "int");

        EXPECT_EQ(enumValue.getName(), "code");
        EXPECT_EQ(enumValue.getType(), "int");
    }

    TEST_F(VniTest, ImportedPropertyStoresTypeStaticFlagAndAccessModifier) {
        const ImportedProperty property("count", "int", true, "protected");

        EXPECT_EQ(property.getName(), "count");
        EXPECT_EQ(property.getType(), "int");
        EXPECT_TRUE(property.isStatic());
        EXPECT_EQ(property.getAccessModifier(), "protected");
    }

    TEST_F(VniTest, ImportedTypeAliasStoresGenericParametersAndOriginalType) {
        const ImportedTypeAlias typeAlias("Wrapper", { "T" }, "List<T>", "public");

        EXPECT_EQ(typeAlias.getName(), "Wrapper");
        EXPECT_EQ(typeAlias.getGenericParameters(), std::vector<std::string>({ "T" }));
        EXPECT_EQ(typeAlias.getOriginalType(), "List<T>");
    }

    TEST_F(VniTest, ImportedFuncStoresReturnTypeNativeFlagAndParameters) {
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>> parameters;
        parameters.emplace("amount", std::make_unique<ImportedParameter>("amount", "int"));

        const ImportedFunc function("add", "add", "int", std::move(parameters), true, "public");

        EXPECT_EQ(function.getOriginalName(), "add");
        EXPECT_EQ(function.getReturnType(), "int");
        EXPECT_TRUE(function.isNative());
        ASSERT_EQ(function.getParameters().size(), 1);
        const auto* parameter = function.getParameterByName("amount");
        ASSERT_NE(parameter, nullptr);
        EXPECT_EQ(parameter->getType(), "int");
        EXPECT_EQ(function.getParameterByName("missing"), nullptr);
    }

    TEST_F(VniTest, ImportedMethodDistinguishesStaticFromNative) {
        const ImportedMethod method("run", "run", "void", {}, true, false, "public");

        EXPECT_EQ(method.getOriginalName(), "run");
        EXPECT_TRUE(method.isStatic());
        EXPECT_FALSE(method.isNative());
    }

    TEST_F(VniTest, ImportedConstructorStoresParametersAndAccessModifier) {
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>> parameters;
        parameters.emplace("amount", std::make_unique<ImportedParameter>("amount", "int"));

        const ImportedConstructor constructor("init", std::move(parameters), "protected");

        EXPECT_EQ(constructor.getAccessModifier(), "protected");
        ASSERT_EQ(constructor.getParameters().size(), 1);
        const auto* parameter = constructor.getParameterByName("amount");
        ASSERT_NE(parameter, nullptr);
        EXPECT_EQ(parameter->getType(), "int");
        EXPECT_EQ(constructor.getParameterByName("missing"), nullptr);
    }

    TEST_F(VniTest, ImportedOperatorStoresReturnTypeParametersNativeFlagAndAccessModifier) {
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>> parameters;
        parameters.emplace("other", std::make_unique<ImportedParameter>("other", "Vec3"));

        const ImportedOperator operatorNode("addition", "Vec3", std::move(parameters), true, "protected");

        EXPECT_EQ(operatorNode.getReturnType(), "Vec3");
        EXPECT_TRUE(operatorNode.isNative());
        EXPECT_EQ(operatorNode.getAccessModifier(), "protected");
        ASSERT_EQ(operatorNode.getParameters().size(), 1);
        const auto* parameter = operatorNode.getParameterByName("other");
        ASSERT_NE(parameter, nullptr);
        EXPECT_EQ(parameter->getType(), "Vec3");
        EXPECT_EQ(operatorNode.getParameterByName("missing"), nullptr);
    }

    TEST_F(VniTest, ImportedClassStoresBaseClassInterfacesAndMembers) {
        std::unordered_map<std::string, std::unique_ptr<ImportedProperty>> properties;
        properties.emplace("count", std::make_unique<ImportedProperty>("count", "int", true, "protected"));

        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>> methods;
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>> parameters;
        methods.emplace("read", std::make_unique<ImportedMethod>("read", "read", "string", std::move(parameters), false, false, "public"));

        std::unordered_map<std::string, std::unique_ptr<ImportedConstructor>> constructors;
        constructors.emplace("init", std::make_unique<ImportedConstructor>("init", std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>{}, "public"));

        std::unordered_map<std::string, std::unique_ptr<ImportedOperator>> operators;
        operators.emplace("addition", std::make_unique<ImportedOperator>("addition", "Box<T>", std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>{}, false, "public"));

        const ImportedClass importedClass(
            "Box",
            std::optional<std::string>("Base"),
            { "Readable" },
            true,
            { "T" },
            std::move(properties),
            std::move(methods),
            std::move(constructors),
            std::move(operators),
            "public"
        );

        EXPECT_EQ(importedClass.getName(), "Box");
        ASSERT_TRUE(importedClass.getBaseClass().has_value());
        EXPECT_EQ(importedClass.getBaseClass().value(), "Base");
        EXPECT_EQ(importedClass.getImplementedInterfaces(), std::vector<std::string>({ "Readable" }));
        EXPECT_TRUE(importedClass.isFinal());
        EXPECT_EQ(importedClass.getGenericParameters(), std::vector<std::string>({ "T" }));

        const auto* property = importedClass.getPropertyByName("count");
        ASSERT_NE(property, nullptr);
        EXPECT_EQ(property->getType(), "int");
        EXPECT_TRUE(property->isStatic());

        const auto* method = importedClass.getMethodByName("read");
        ASSERT_NE(method, nullptr);
        EXPECT_EQ(method->getOriginalName(), "read");
        EXPECT_EQ(method->getReturnType(), "string");
        EXPECT_FALSE(method->isStatic());
        EXPECT_FALSE(method->isNative());

        const auto* constructor = importedClass.getConstructorByName("init");
        ASSERT_NE(constructor, nullptr);
        EXPECT_EQ(constructor->getAccessModifier(), "public");

        const auto* operatorNode = importedClass.getOperatorByName("addition");
        ASSERT_NE(operatorNode, nullptr);
        EXPECT_EQ(operatorNode->getReturnType(), "Box<T>");
    }

    TEST_F(VniTest, ImportedClassAllowsNoBaseClassAndEmptyMembers) {
        const ImportedClass importedClass("Empty", std::nullopt, {}, false, {}, {}, {}, {}, "public");

        EXPECT_FALSE(importedClass.getBaseClass().has_value());
        EXPECT_TRUE(importedClass.getImplementedInterfaces().empty());
        EXPECT_FALSE(importedClass.isFinal());
        EXPECT_TRUE(importedClass.getGenericParameters().empty());
        EXPECT_TRUE(importedClass.getProperties().empty());
        EXPECT_TRUE(importedClass.getMethods().empty());
        EXPECT_TRUE(importedClass.getConstructors().empty());
        EXPECT_TRUE(importedClass.getOperators().empty());
    }

    TEST_F(VniTest, ImportedInterfaceStoresGenericParametersAndMethods) {
        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>> methods;
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>> parameters;
        methods.emplace("read", std::make_unique<ImportedMethod>("read", "read", "string", std::move(parameters), false, false, "public"));

        std::unordered_map<std::string, std::unique_ptr<ImportedOperator>> operators;
        operators.emplace("equality", std::make_unique<ImportedOperator>("equality", "bool", std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>{}, false, "public"));

        const ImportedInterface importedInterface("Readable", { "T" }, std::move(methods), std::move(operators), "public");

        EXPECT_EQ(importedInterface.getGenericParameters(), std::vector<std::string>({ "T" }));
        const auto* method = importedInterface.getMethodByName("read");
        ASSERT_NE(method, nullptr);
        EXPECT_EQ(method->getOriginalName(), "read");
        EXPECT_EQ(method->getReturnType(), "string");

        const auto* operatorNode = importedInterface.getOperatorByName("equality");
        ASSERT_NE(operatorNode, nullptr);
        EXPECT_EQ(operatorNode->getReturnType(), "bool");
    }

    TEST_F(VniTest, ImportedEnumStoresGenericParametersAndMembers) {
        std::unordered_map<std::string, std::unique_ptr<ImportedEnumValue>> associatedValues;
        associatedValues.emplace("code", std::make_unique<ImportedEnumValue>("code", "int"));

        std::unordered_map<std::string, std::unique_ptr<ImportedEnumMember>> members;
        members.emplace("Ready", std::make_unique<ImportedEnumMember>("Ready", std::move(associatedValues)));

        const ImportedEnum importedEnum("State", { "T" }, std::move(members), "public");

        EXPECT_EQ(importedEnum.getGenericParameters(), std::vector<std::string>({ "T" }));
        const auto* member = importedEnum.getMemberByName("Ready");
        ASSERT_NE(member, nullptr);
        ASSERT_EQ(member->getAssociatedValues().size(), 1);
        EXPECT_EQ(member->getAssociatedValues().at("code")->getType(), "int");
    }

    TEST_F(VniTest, ImportedModuleExposesNameAndIdentifiers) {
        std::unordered_map<std::string, std::unique_ptr<ImportedIdentifier>> identifiers;
        identifiers.emplace("value", std::make_unique<ImportedLet>("value", "int", "public"));

        const ImportedModule module("module", std::move(identifiers));

        EXPECT_EQ(module.getName(), "module");
        const auto* identifier = module.getIdentifierByName("value");
        ASSERT_NE(identifier, nullptr);
        EXPECT_EQ(dynamic_cast<const ImportedLet*>(identifier)->getType(), "int");
        EXPECT_EQ(module.getIdentifierByName("missing"), nullptr);
    }

    TEST_F(VniTest, ImportedModuleDoesNotOverwriteExistingIdentifier) {
        ImportedModule module("module", {});

        module.addIdentifier(std::make_unique<ImportedLet>("value", "int", "public"));
        module.addIdentifier(std::make_unique<ImportedLet>("value", "string", "public"));

        ASSERT_EQ(module.getIdentifiers().size(), 1);
        const auto* identifier = module.getIdentifierByName("value");
        ASSERT_NE(identifier, nullptr);
        EXPECT_EQ(dynamic_cast<const ImportedLet*>(identifier)->getType(), "int");
    }

    TEST_F(VniTest, ImportedPackageExposesSubPackagesAndModules) {
        std::unordered_map<std::string, std::unique_ptr<ImportedModule>> modules;
        modules.emplace("api", std::make_unique<ImportedModule>("api", std::unordered_map<std::string, std::unique_ptr<ImportedIdentifier>>{}));

        const ImportedPackage package("package", {}, std::move(modules));

        const auto* module = package.getModuleByName("api");
        ASSERT_NE(module, nullptr);
        EXPECT_EQ(module->getName(), "api");
        EXPECT_EQ(package.getModuleByName("missing"), nullptr);
        EXPECT_EQ(package.getSubPackageByName("missing"), nullptr);
    }

    TEST_F(VniTest, ImportedPackageDoesNotOverwriteExistingModule) {
        ImportedPackage package("package", {}, {});

        package.addModule(std::make_unique<ImportedModule>("api", std::unordered_map<std::string, std::unique_ptr<ImportedIdentifier>>{}));
        package.addModule(std::make_unique<ImportedModule>("api", std::unordered_map<std::string, std::unique_ptr<ImportedIdentifier>>{}));

        ASSERT_EQ(package.getModules().size(), 1);
    }

    TEST_F(VniTest, ImportedItemResolvesFullTypeNameThroughNestedPackages) {
        const auto imports = makeNestedImports();

        const auto* item = imports.getImportedItemByFullTypeName("root.sub.api.State");

        ASSERT_NE(item, nullptr);
        EXPECT_NE(dynamic_cast<const ImportedEnum*>(item), nullptr);
        EXPECT_EQ(item->getName(), "State");
    }

    TEST_F(VniTest, ImportedItemIgnoresGenericArgumentsAndSurroundingSpacesInFullTypeName) {
        const auto imports = makeNestedImports();
        const auto* expected = imports.getImportedItemByFullTypeName("root.sub.api.State");
        ASSERT_NE(expected, nullptr);

        EXPECT_EQ(imports.getImportedItemByFullTypeName("root.sub.api.State<int>"), expected);
        EXPECT_EQ(imports.getImportedItemByFullTypeName("root.sub.api.State<root.sub.api.State<int>, string>"), expected);
        EXPECT_EQ(imports.getImportedItemByFullTypeName(" \troot.sub.api.State <int>\n"), expected);
    }

    TEST_F(VniTest, ImportedItemReturnsNullForUnresolvableFullTypeName) {
        const auto imports = makeNestedImports();

        EXPECT_EQ(imports.getImportedItemByFullTypeName(""), nullptr);
        EXPECT_EQ(imports.getImportedItemByFullTypeName("   "), nullptr);
        EXPECT_EQ(imports.getImportedItemByFullTypeName("<int>"), nullptr);
        EXPECT_EQ(imports.getImportedItemByFullTypeName("missing.sub.api.State"), nullptr);
        EXPECT_EQ(imports.getImportedItemByFullTypeName("root.sub.api.Missing"), nullptr);
        EXPECT_EQ(imports.getImportedItemByFullTypeName("root..api.State"), nullptr);
        EXPECT_EQ(imports.getImportedItemByFullTypeName("root.sub.api.State."), nullptr);
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderReadsModuleInterfaceFile) {
        const auto filePath = writeFile(
            "dependency/package/api.vni",
            R"({
    "value": {
        "category": "let",
        "accessModifier": "private",
        "type": "int",
        "metadata": {
            "since": "1.0",
            "draft": null
        }
    },
    "run": {
        "category": "func",
        "accessModifier": "public",
        "name": "run",
        "returnType": "void",
        "parameters": {
            "amount": {
                "category": "parameter",
                "type": "int"
            }
        },
        "native": true
    },
    "standaloneMethod": {
        "category": "method",
        "name": "standaloneMethod",
        "returnType": "string",
        "parameters": {},
        "static": true,
        "native": false,
        "accessModifier": "public"
    },
    "Box": {
        "category": "class",
        "accessModifier": "public",
        "genericParameters": ["T"],
        "properties": {
            "count": {
                "category": "property",
                "type": "int",
                "static": true,
                "accessModifier": "protected"
            }
        },
        "methods": {
            "read": {
                "category": "method",
                "name": "read",
                "returnType": "T",
                "parameters": {
                    "index": {
                        "category": "parameter",
                        "type": "int"
                    }
                },
                "static": false,
                "native": false,
                "accessModifier": "public"
            }
        },
        "constructors": {
            "init": {
                "category": "constructor",
                "parameters": {
                    "amount": {
                        "category": "parameter",
                        "type": "int"
                    }
                },
                "accessModifier": "protected"
            }
        },
        "operators": {
            "addition": {
                "category": "operator",
                "returnType": "Box<T>",
                "parameters": {
                    "index": {
                        "category": "parameter",
                        "type": "int"
                    }
                },
                "native": true,
                "accessModifier": "public"
            }
        },
        "baseClass": "Base",
        "implementedInterfaces": ["Readable"],
        "final": true
    },
    "Readable": {
        "category": "interface",
        "accessModifier": "public",
        "genericParameters": ["T"],
        "methods": {
            "read": {
                "category": "method",
                "name": "read",
                "returnType": "T",
                "parameters": {},
                "static": false,
                "native": false,
                "accessModifier": "public"
            }
        },
        "operators": {
            "equality": {
                "category": "operator",
                "returnType": "bool",
                "parameters": {},
                "native": false,
                "accessModifier": "public"
            }
        }
    },
    "State": {
        "category": "enum",
        "accessModifier": "public",
        "genericParameters": [],
        "members": {
            "Ready": {
                "category": "enummember",
                "associatedValues": {
                    "code": {
                        "category": "enumvalue",
                        "type": "int"
                    }
                }
            }
        }
    },
    "Alias": {
        "category": "typealias",
        "accessModifier": "public",
        "genericParameters": ["T"],
        "originalType": "List<T>"
    },
    "standaloneProperty": {
        "category": "property",
        "type": "string",
        "static": false,
        "accessModifier": "private"
    },
    "standaloneParameter": {
        "category": "parameter",
        "type": "string"
    }
})"
        );

        ModuleInterfaceFileReader reader(filePath);
        const auto module = reader.read();

        ASSERT_EQ(module->getName(), "api");
        ASSERT_EQ(module->getIdentifiers().size(), 9);

        const auto* value = module->getIdentifierByName("value");
        ASSERT_NE(value, nullptr);
        EXPECT_EQ(dynamic_cast<const ImportedLet*>(value)->getType(), "int");
        EXPECT_EQ(value->getAccessModifier(), "private");
        for (const auto name : { "run", "standaloneMethod", "Box", "Readable", "State", "Alias" }) {
            SCOPED_TRACE(name);
            const auto* identifier = module->getIdentifierByName(name);
            ASSERT_NE(identifier, nullptr);
            EXPECT_EQ(identifier->getAccessModifier(), "public");
        }

        const auto* function = module->getIdentifierByName("run");
        ASSERT_NE(function, nullptr);
        const auto* importedFunction = dynamic_cast<const ImportedFunc*>(function);
        ASSERT_NE(importedFunction, nullptr);
        EXPECT_EQ(importedFunction->getOriginalName(), "run");
        EXPECT_EQ(importedFunction->getReturnType(), "void");
        EXPECT_TRUE(importedFunction->isNative());
        const auto* parameter = importedFunction->getParameterByName("amount");
        ASSERT_NE(parameter, nullptr);
        EXPECT_EQ(parameter->getType(), "int");

        const auto* standaloneMethod = module->getIdentifierByName("standaloneMethod");
        ASSERT_NE(standaloneMethod, nullptr);
        const auto* importedStandaloneMethod = dynamic_cast<const ImportedMethod*>(standaloneMethod);
        ASSERT_NE(importedStandaloneMethod, nullptr);
        EXPECT_EQ(importedStandaloneMethod->getOriginalName(), "standaloneMethod");

        const auto* importedClassIdentifier = module->getIdentifierByName("Box");
        ASSERT_NE(importedClassIdentifier, nullptr);
        const auto* importedClass = dynamic_cast<const ImportedClass*>(importedClassIdentifier);
        ASSERT_NE(importedClass, nullptr);
        ASSERT_TRUE(importedClass->getBaseClass().has_value());
        EXPECT_EQ(importedClass->getBaseClass().value(), "Base");
        EXPECT_EQ(importedClass->getImplementedInterfaces(), std::vector<std::string>({ "Readable" }));
        EXPECT_TRUE(importedClass->isFinal());
        const auto* property = importedClass->getPropertyByName("count");
        ASSERT_NE(property, nullptr);
        EXPECT_TRUE(property->isStatic());
        EXPECT_EQ(property->getAccessModifier(), "protected");
        const auto* method = importedClass->getMethodByName("read");
        ASSERT_NE(method, nullptr);
        EXPECT_EQ(method->getOriginalName(), "read");
        EXPECT_EQ(method->getReturnType(), "T");
        EXPECT_FALSE(method->isStatic());
        EXPECT_FALSE(method->isNative());
        const auto* constructor = importedClass->getConstructorByName("init");
        ASSERT_NE(constructor, nullptr);
        EXPECT_EQ(constructor->getAccessModifier(), "protected");
        const auto* constructorParameter = constructor->getParameterByName("amount");
        ASSERT_NE(constructorParameter, nullptr);
        EXPECT_EQ(constructorParameter->getType(), "int");

        const auto* classOperator = importedClass->getOperatorByName("addition");
        ASSERT_NE(classOperator, nullptr);
        EXPECT_EQ(classOperator->getReturnType(), "Box<T>");
        EXPECT_TRUE(classOperator->isNative());
        const auto* operatorParameter = classOperator->getParameterByName("index");
        ASSERT_NE(operatorParameter, nullptr);
        EXPECT_EQ(operatorParameter->getType(), "int");

        const auto* importedInterfaceIdentifier = module->getIdentifierByName("Readable");
        ASSERT_NE(importedInterfaceIdentifier, nullptr);
        const auto* importedInterface = dynamic_cast<const ImportedInterface*>(importedInterfaceIdentifier);
        ASSERT_NE(importedInterface, nullptr);
        EXPECT_EQ(importedInterface->getGenericParameters(), std::vector<std::string>({ "T" }));
        const auto* interfaceMethod = importedInterface->getMethodByName("read");
        ASSERT_NE(interfaceMethod, nullptr);
        EXPECT_EQ(interfaceMethod->getOriginalName(), "read");
        const auto* interfaceOperator = importedInterface->getOperatorByName("equality");
        ASSERT_NE(interfaceOperator, nullptr);
        EXPECT_EQ(interfaceOperator->getReturnType(), "bool");
        EXPECT_FALSE(interfaceOperator->isNative());

        const auto* importedEnumIdentifier = module->getIdentifierByName("State");
        ASSERT_NE(importedEnumIdentifier, nullptr);
        const auto* importedEnum = dynamic_cast<const ImportedEnum*>(importedEnumIdentifier);
        ASSERT_NE(importedEnum, nullptr);
        const auto* enumMember = importedEnum->getMemberByName("Ready");
        ASSERT_NE(enumMember, nullptr);
        ASSERT_EQ(enumMember->getAssociatedValues().size(), 1);
        EXPECT_EQ(enumMember->getAssociatedValues().at("code")->getType(), "int");

        const auto* typeAliasIdentifier = module->getIdentifierByName("Alias");
        ASSERT_NE(typeAliasIdentifier, nullptr);
        const auto* typeAlias = dynamic_cast<const ImportedTypeAlias*>(typeAliasIdentifier);
        ASSERT_NE(typeAlias, nullptr);
        EXPECT_EQ(typeAlias->getOriginalType(), "List<T>");
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderThrowsOnEmptyFile) {
        const auto filePath = writeFile("empty.vni", "");
        ModuleInterfaceFileReader reader(filePath);

        EXPECT_THROW(static_cast<void>(reader.read()), ModuleInterfaceFileReaderError);
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderThrowsOnDirectory) {
        const auto directory = testDirectory / "directory.vni";
        std::filesystem::create_directories(directory);
        ModuleInterfaceFileReader reader(directory);

        EXPECT_THROW(static_cast<void>(reader.read()), ModuleInterfaceFileReaderError);
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderThrowsOnWrongExtension) {
        const auto filePath = writeFile("module.vnl", "{}");
        ModuleInterfaceFileReader reader(filePath);

        EXPECT_THROW(static_cast<void>(reader.read()), ModuleInterfaceFileReaderError);
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderAcceptsCaseInsensitiveExtensionsAndKeepsModuleNameCase) {
        for (const auto& [fileName, moduleName] : { std::pair{ "Upper.VNI", "Upper" }, std::pair{ "Mixed.Vni", "Mixed" }, std::pair{ "lower.vni", "lower" } }) {
            SCOPED_TRACE(fileName);
            const auto filePath = writeFile(fileName, R"({"value":{"category":"let","accessModifier":"public","type":"int"}})");
            ModuleInterfaceFileReader reader(filePath);

            const auto module = reader.read();
            EXPECT_EQ(module->getName(), moduleName);
            EXPECT_NE(module->getIdentifierByName("value"), nullptr);
        }
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderThrowsOnInvalidJson) {
        const auto filePath = writeFile("module.vni", "{");
        ModuleInterfaceFileReader reader(filePath);

        EXPECT_THROW(static_cast<void>(reader.read()), ModuleInterfaceFileReaderError);
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderThrowsOnUnknownCategory) {
        for (const auto contents : {
                 R"({"value":{"category":"unknown"}})",
                 R"({"External":{"category":"imported","source":"dependency.module.External"}})",
             }) {
            SCOPED_TRACE(contents);
            const auto filePath = writeFile("module.vni", contents);
            ModuleInterfaceFileReader reader(filePath);

            EXPECT_THROW(static_cast<void>(reader.read()), ModuleInterfaceFileReaderError);
        }
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderThrowsWhenFunctionOrMethodLacksName) {
        for (const auto contents : {
                 R"({"run":{"category":"func","accessModifier":"public","returnType":"void","parameters":{},"native":false}})",
                 R"({"run":{"category":"method","returnType":"void","parameters":{},"native":false,"static":false,"accessModifier":"public"}})",
             }) {
            SCOPED_TRACE(contents);
            const auto filePath = writeFile("module.vni", contents);
            ModuleInterfaceFileReader reader(filePath);

            EXPECT_THROW(static_cast<void>(reader.read()), ModuleInterfaceFileReaderError);
        }
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderThrowsWhenOperatorLacksValidNativeFlag) {
        for (const auto contents : {
                 R"({"addition":{"category":"operator","returnType":"int","parameters":{},"accessModifier":"public"}})",
                 R"({"addition":{"category":"operator","returnType":"int","parameters":{},"native":"true","accessModifier":"public"}})",
             }) {
            SCOPED_TRACE(contents);
            const auto filePath = writeFile("module.vni", contents);
            ModuleInterfaceFileReader reader(filePath);

            EXPECT_THROW(static_cast<void>(reader.read()), ModuleInterfaceFileReaderError);
        }
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderThrowsWhenTopLevelIdentifierLacksValidAccessModifier) {
        for (const auto category : {
                 R"("category":"let","type":"int")",
                 R"("category":"func","name":"run","returnType":"void","parameters":{},"native":false)",
                 R"("category":"class","genericParameters":[],"properties":{},"methods":{},"constructors":{},"operators":{},"baseClass":null,"implementedInterfaces":[],"final":false)",
                 R"("category":"interface","genericParameters":[],"methods":{},"operators":{})",
                 R"("category":"enum","genericParameters":[],"members":{})",
                 R"("category":"typealias","genericParameters":[],"originalType":"int")",
             }) {
            for (const auto accessModifierField : { "", R"(,"accessModifier":"protected")", R"(,"accessModifier":"internal")", R"(,"accessModifier":false)", R"(,"accessModifier":null)" }) {
                const auto contents = std::string(R"({"identifier":{)") + category + accessModifierField + "}}";
                SCOPED_TRACE(contents);
                const auto filePath = writeFile("module.vni", contents);
                ModuleInterfaceFileReader reader(filePath);

                try {
                    static_cast<void>(reader.read());
                    ADD_FAILURE() << "Expected ModuleInterfaceFileReaderError";
                } catch (const ModuleInterfaceFileReaderError& error) {
                    EXPECT_NE(std::string_view(error.what()).find("'accessModifier' field"), std::string_view::npos) << error.what();
                }
            }
        }
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderThrowsOnNonexistentFile) {
        EXPECT_THROW(ModuleInterfaceFileReader(testDirectory / "missing.vni"), std::filesystem::filesystem_error);
    }

    TEST_F(VniTest, PackageReaderThrowsOnMissingPackage) {
        const auto dependencyRoot = testDirectory / "dependencies";
        std::filesystem::create_directories(dependencyRoot / "available");

        Config config{};
        config.dependencyPackageRootPaths.emplace("available", dependencyRoot / "available");

        ImportedRootPackageMap packages;
        PackageReader reader(packages);

        EXPECT_THROW(reader.readPackageFromPath("missing.module", config), PackageReaderError);
    }

    TEST_F(VniTest, PackageReaderReadsModuleInsidePackage) {
        const auto packageRoot = testDirectory / "dependencies" / "package";
        const auto modulePath = packageRoot / "api.vni";
        std::filesystem::create_directories(packageRoot);
        {
            std::ofstream output(modulePath);
            output << R"({
    "value": {
        "category": "let",
        "accessModifier": "public",
        "type": "int"
    }
})";
        }

        Config config{};
        config.dependencyPackageRootPaths.emplace("package", packageRoot);

        ImportedRootPackageMap packages;
        PackageReader reader(packages);
        reader.readPackageFromPath("package.api", config);

        const auto package = packages.find("package");
        ASSERT_NE(package, packages.end());
        const auto* module = package->second->getModuleByName("api");
        ASSERT_NE(module, nullptr);
        ASSERT_EQ(module->getIdentifiers().size(), 1);
        EXPECT_EQ(dynamic_cast<const ImportedLet*>(module->getIdentifiers().at("value").get())->getType(), "int");
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorGeneratesLetCategory) {
        const auto config = makeGeneratorConfig(testDirectory);
        auto type = makeType("int");
        const auto* typeNode = type.get();
        auto declaration = makeValueWithMetadata(
            ValueDeclarationKind::Kind::LET,
            ValueDeclarationKind::Context::TOP_LEVEL,
            ValueDeclarationKind::AccessModifier::PUBLIC,
            "count",
            std::move(type),
            makeMetadata()
        );
        const auto* declarationNode = declaration.get();
        const auto semantic = makeSemanticResult({ { typeNode, PrimitiveType::intType() } });

        ModuleInterfaceFileGenerator generator({ declarationNode }, config, semantic);
        generator.generate();
        const auto json = readJson(generatedOutputPath(config));

        EXPECT_EQ(json["count"]["category"], "let");
        EXPECT_EQ(json["count"]["type"], "int");
        EXPECT_EQ(json["count"]["accessModifier"], "public");
        EXPECT_EQ(json["count"]["metadata"]["since"], "1.0");
        EXPECT_TRUE(json["count"]["metadata"]["draft"].is_null());
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorGeneratesAccessModifiersOfTopLevelDeclarations) {
        struct AccessModifierCase {
            ValueDeclarationKind::AccessModifier valueAccessModifier;
            FunctionDeclarationKind::AccessModifier functionAccessModifier;
            TypeDeclarationKind::AccessModifier typeAccessModifier;
            std::string_view expected;
        };
        const std::vector<AccessModifierCase> cases = {
            { ValueDeclarationKind::AccessModifier::PUBLIC, FunctionDeclarationKind::AccessModifier::PUBLIC, TypeDeclarationKind::AccessModifier::PUBLIC, "public" },
            { ValueDeclarationKind::AccessModifier::PRIVATE, FunctionDeclarationKind::AccessModifier::PRIVATE, TypeDeclarationKind::AccessModifier::PRIVATE, "private" },
        };
        for (const auto& [valueAccessModifier, functionAccessModifier, typeAccessModifier, expected] : cases) {
            SCOPED_TRACE(expected);
            const auto config = makeGeneratorConfig(testDirectory);

            auto variableType = makeType("int");
            auto originalType = makeType("int");
            const auto semantic = makeSemanticResult(
                {
                    { variableType.get(), PrimitiveType::intType() },
                    { originalType.get(), PrimitiveType::intType() },
                }
            );
            const auto variable = makeValue(ValueDeclarationKind::Kind::LET, ValueDeclarationKind::Context::TOP_LEVEL, valueAccessModifier, "count", std::move(variableType));
            const auto function = makeFunction(functionAccessModifier, "run");
            const auto classNode = makeClass(typeAccessModifier, "Box");
            const auto interfaceNode = makeInterface(typeAccessModifier, "Readable");
            const auto enumNode = makeEnum(typeAccessModifier, "State", {}, {});
            const auto typeAlias = makeTypeAlias(typeAccessModifier, "Alias", {}, std::move(originalType));

            ModuleInterfaceFileGenerator generator({ variable.get(), function.get(), classNode.get(), interfaceNode.get(), enumNode.get(), typeAlias.get() }, config, semantic);
            generator.generate();
            const auto json = readJson(generatedOutputPath(config));

            ASSERT_EQ(json.size(), 6);
            for (const auto name : { "count", "run", "Box", "Readable", "State", "Alias" }) {
                SCOPED_TRACE(name);
                EXPECT_EQ(json[name]["accessModifier"].get<std::string>(), expected);
            }
        }
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorGeneratesEnumWithMemberAndValue) {
        const auto config = makeGeneratorConfig(testDirectory);
        auto valueType = makeType("int");
        const auto* valueTypeAnnotationNode = valueType.get();

        std::vector<std::unique_ptr<ValueDeclarationNode>> associatedValues;
        associatedValues.push_back(
            makeValue(ValueDeclarationKind::Kind::ENUM_ASSOCIATED_VALUE, ValueDeclarationKind::Context::ENUM_MEMBER, ValueDeclarationKind::AccessModifier::PUBLIC, "code", std::move(valueType))
        );

        std::vector<std::unique_ptr<EnumMemberDeclarationNode>> members;
        members.push_back(std::make_unique<EnumMemberDeclarationNode>(makeIdentifier("Ready"), std::move(associatedValues), testToken, testToken));

        std::vector<std::unique_ptr<IdentifierNode>> genericParameters;
        genericParameters.push_back(makeIdentifier("T"));

        auto declaration = makeEnum(TypeDeclarationKind::AccessModifier::PUBLIC, "State", std::move(genericParameters), std::move(members));
        const auto* declarationNode = declaration.get();
        const auto semantic = makeSemanticResult(
            {
                { valueTypeAnnotationNode, PrimitiveType::intType() },
            }
        );

        ModuleInterfaceFileGenerator generator({ declarationNode }, config, semantic);
        generator.generate();
        const auto json = readJson(generatedOutputPath(config));

        EXPECT_EQ(json["State"]["category"], "enum");
        EXPECT_EQ(json["State"]["accessModifier"], "public");
        EXPECT_EQ(json["State"]["genericParameters"].size(), 1);
        EXPECT_EQ(json["State"]["genericParameters"][0], "T");
        EXPECT_EQ(json["State"]["members"]["Ready"]["category"], "enummember");
        EXPECT_EQ(json["State"]["members"]["Ready"]["associatedValues"]["code"]["category"], "enumvalue");
        EXPECT_EQ(json["State"]["members"]["Ready"]["associatedValues"]["code"]["type"], "int");
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorGeneratesTypeAliasCategory) {
        const auto config = makeGeneratorConfig(testDirectory);
        auto originalType = makeType("List<T>");
        const auto* originalTypeAnnotationNode = originalType.get();

        std::vector<std::unique_ptr<IdentifierNode>> genericParameters;
        genericParameters.push_back(makeIdentifier("T"));

        auto declaration = makeTypeAlias(TypeDeclarationKind::AccessModifier::PUBLIC, "Wrapper", std::move(genericParameters), std::move(originalType));
        const auto* declarationNode = declaration.get();
        const auto semantic = makeSemanticResult(
            {
                { originalTypeAnnotationNode, PrimitiveType::intType() },
            }
        );

        ModuleInterfaceFileGenerator generator({ declarationNode }, config, semantic);
        generator.generate();
        const auto json = readJson(generatedOutputPath(config));

        EXPECT_EQ(json["Wrapper"]["category"], "typealias");
        EXPECT_EQ(json["Wrapper"]["accessModifier"], "public");
        EXPECT_EQ(json["Wrapper"]["genericParameters"].size(), 1);
        EXPECT_EQ(json["Wrapper"]["genericParameters"][0], "T");
        EXPECT_EQ(json["Wrapper"]["originalType"], "int");
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorUsesInferredTypes) {
        const auto config = makeGeneratorConfig(testDirectory);
        auto declaration = makeValue(ValueDeclarationKind::Kind::LET, ValueDeclarationKind::Context::TOP_LEVEL, ValueDeclarationKind::AccessModifier::PUBLIC, "count", nullptr);
        const auto* declarationNode = declaration.get();
        const auto semantic = makeSemanticResult(
            {},
            {
                { declarationNode, PrimitiveType::intType() },
            }
        );

        ModuleInterfaceFileGenerator generator({ declarationNode }, config, semantic);
        generator.generate();
        const auto json = readJson(generatedOutputPath(config));

        EXPECT_EQ(json["count"]["type"], "int");
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorGeneratesNativeFlagForOperators) {
        const auto config = makeGeneratorConfig(testDirectory);

        std::vector<std::unique_ptr<OperatorDeclarationNode>> operators;
        operators.push_back(
            std::make_unique<OperatorDeclarationNode>(
                OperatorDeclarationKind::Kind::ADDITION,
                OperatorDeclarationKind::Implementation::NATIVE,
                OperatorDeclarationKind::Context::CLASS,
                OperatorDeclarationKind::AccessModifier::PUBLIC,
                std::vector<std::unique_ptr<ValueDeclarationNode>>{},
                nullptr,
                nullptr,
                testToken,
                testToken
            )
        );
        operators.push_back(
            std::make_unique<OperatorDeclarationNode>(
                OperatorDeclarationKind::Kind::SUBTRACTION,
                OperatorDeclarationKind::Implementation::REGULAR,
                OperatorDeclarationKind::Context::CLASS,
                OperatorDeclarationKind::AccessModifier::PUBLIC,
                std::vector<std::unique_ptr<ValueDeclarationNode>>{},
                nullptr,
                std::make_unique<BlockStatementNode>(std::vector<std::unique_ptr<StatementNode>>{}, testToken, testToken),
                testToken,
                testToken
            )
        );
        operators[0]->setInternalName("vnl-operator-addition()");
        operators[1]->setInternalName("vnl-operator-subtraction()");

        auto declaration = std::make_unique<ClassDeclarationNode>(
            TypeDeclarationKind::AccessModifier::PUBLIC,
            false,
            makeIdentifier("Vec3"),
            nullptr,
            std::vector<std::unique_ptr<TypeAnnotationNode>>{},
            std::vector<std::unique_ptr<IdentifierNode>>{},
            std::vector<std::unique_ptr<ValueDeclarationNode>>{},
            std::vector<std::unique_ptr<FunctionDeclarationNode>>{},
            std::vector<std::unique_ptr<ConstructorDeclarationNode>>{},
            std::move(operators),
            testToken,
            testToken
        );
        const auto* declarationNode = declaration.get();
        const auto semantic = makeSemanticResult();

        ModuleInterfaceFileGenerator generator({ declarationNode }, config, semantic);
        generator.generate();
        const auto json = readJson(generatedOutputPath(config));

        EXPECT_EQ(json["Vec3"]["operators"]["vnl-operator-addition()"]["native"], true);
        EXPECT_EQ(json["Vec3"]["operators"]["vnl-operator-subtraction()"]["native"], false);
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorDoesNotWriteWhenOutputDirectoryIsAbsent) {
        auto config = makeGeneratorConfig(testDirectory);
        config.moduleInterfaceOutputDirectory.clear();
        const auto semantic = makeSemanticResult();

        ModuleInterfaceFileGenerator generator({}, config, semantic);
        generator.generate();

        EXPECT_FALSE(std::filesystem::exists(testDirectory / "interfaces"));
    }

} // namespace vnlc
