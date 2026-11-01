#include "parser/Parser.hpp"
#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/declaration/ConstructorDeclarationNode.hpp"
#include "ast/declaration/EnumDeclarationNode.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include "ast/declaration/OperatorDeclarationNode.hpp"
#include "ast/declaration/TypeAliasDeclarationNode.hpp"
#include "ast/declaration/TypeDeclarationKind.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/expression/BinaryExpressionNode.hpp"
#include "ast/expression/IdentifierLikeExpressionNode.hpp"
#include "ast/expression/MemberAccessExpressionNode.hpp"
#include "ast/expression/NoneExpressionNode.hpp"
#include "ast/expression/PrimitiveTypeExpressionKind.hpp"
#include "ast/expression/PrimitiveTypeExpressionNode.hpp"
#include "ast/expression/RangeExpressionNode.hpp"
#include "ast/statement/ForStatementNode.hpp"
#include "ast/statement/VariableDeclarationStatementNode.hpp"
#include "ast/typeannotation/CustomizedTypeAnnotationNode.hpp"
#include "ast/typeannotation/PrimitiveTypeAnnotationKind.hpp"
#include "ast/typeannotation/PrimitiveTypeAnnotationNode.hpp"
#include "collector/Collector.hpp"
#include "config/Config.hpp"
#include "error/SyntaxError.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <string_view>

namespace vnlc {

    namespace {

        CollectionResult collectModule(std::istream& input, const Config& config) {
            Lexer lexer(input);
            Collector collector(std::move(lexer));
            return collector.collect(config);
        }

        ParseResult parseModule(std::istream& input, const Config& config) {
            auto collectionResult = collectModule(input, config);

            input.clear();
            input.seekg(0);

            Lexer lexer(input);
            Parser parser(std::move(lexer), collectionResult);
            return parser.parse(config);
        }

        std::filesystem::path testPackageRootPath() {
            return std::filesystem::canonical(std::filesystem::path(VNLC_TEST_SOURCE_DIR) / "inputs");
        }

    } // namespace

    TEST(ParserTest, SimpleModule) {
        const auto inputDir = std::filesystem::path(VNLC_TEST_SOURCE_DIR) / "inputs";

        std::ifstream input(inputDir / "main.vnl");

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = std::filesystem::canonical(inputDir),
            .inputFilePath = std::filesystem::canonical(inputDir / "main.vnl"),
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
    }

    TEST(ParserTest, StripsSourceExtensionsCaseInsensitively) {
        for (const auto fileName : { "sample.vnl", "sample.VNL", "sample.Vnl" }) {
            SCOPED_TRACE(fileName);
            std::stringstream input("class Item {}\n");

            Config config{
                .mode = RunningMode::COMPILE,
                .vanillangVersion = "1.0",
                .minecraftVersion = "26.1.2",
                .packageRootPath = testPackageRootPath(),
                .inputFilePath = testPackageRootPath() / fileName,
                .outputDirectory = {},
                .dependencyPackageRootPaths = {},
                .optimizationLevel = std::nullopt,
            };

            auto result = parseModule(input, config);
            EXPECT_EQ(result.getModuleNode().getName(), "sample");
            EXPECT_TRUE(result.getModuleNode().getFullName().ends_with(".sample"));
        }
    }

    TEST(ParserTest, DistinguishesLoopVariablesFromOrdinaryVariables) {
        std::stringstream input("func test() {\n    let value = 1\n    for (let item in 1..3) {}\n}\n");

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 1);

        const auto* function = dynamic_cast<const FunctionDeclarationNode*>(module.getTopIdentifierDeclarations().front().get());
        ASSERT_NE(function, nullptr);
        ASSERT_NE(function->getBody(), nullptr);

        const auto& statements = function->getBody()->getStatements();
        ASSERT_EQ(statements.size(), 2);

        const auto* variableStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[0].get());
        ASSERT_NE(variableStatement, nullptr);
        EXPECT_EQ(variableStatement->getVariableDeclaration().getKind(), ValueDeclarationKind::Kind::LET);
        EXPECT_NE(variableStatement->getVariableDeclaration().getInitializer(), nullptr);

        const auto* forStatement = dynamic_cast<const ForStatementNode*>(statements[1].get());
        ASSERT_NE(forStatement, nullptr);
        EXPECT_EQ(forStatement->getLoopVariable().getKind(), ValueDeclarationKind::Kind::LOOP_VARIABLE);
        EXPECT_EQ(forStatement->getLoopVariable().getInitializer(), nullptr);
    }

    TEST(ParserTest, PropertyDeclarationsUseValueDeclarationNode) {
        std::stringstream input("class Example {\n    private value: string\n    static count: int = 0\n}\n");

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 1);

        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(module.getTopIdentifierDeclarations().front().get());
        ASSERT_NE(classDeclaration, nullptr);
        ASSERT_EQ(classDeclaration->getPropertyDeclarations().size(), 2);

        const auto* instanceProperty = classDeclaration->getPropertyDeclarations()[0].get();
        EXPECT_EQ(instanceProperty->getKind(), ValueDeclarationKind::Kind::INSTANCE_PROPERTY);
        EXPECT_EQ(instanceProperty->getContext(), ValueDeclarationKind::Context::CLASS);
        EXPECT_EQ(instanceProperty->getAccessModifier(), ValueDeclarationKind::AccessModifier::PRIVATE);
        EXPECT_NE(instanceProperty->getType(), nullptr);
        EXPECT_EQ(instanceProperty->getInitializer(), nullptr);

        const auto* staticProperty = classDeclaration->getPropertyDeclarations()[1].get();
        EXPECT_EQ(staticProperty->getKind(), ValueDeclarationKind::Kind::STATIC_PROPERTY);
        EXPECT_EQ(staticProperty->getContext(), ValueDeclarationKind::Context::CLASS);
        EXPECT_EQ(staticProperty->getAccessModifier(), ValueDeclarationKind::AccessModifier::PUBLIC);
        EXPECT_NE(staticProperty->getType(), nullptr);
        EXPECT_NE(staticProperty->getInitializer(), nullptr);
    }

    TEST(ParserTest, ConstructorDeclarationsUseConstructorDeclarationNode) {
        std::stringstream input("class Example {\n    protected init(value: string) {\n        let copy = value\n    }\n}\n");

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 1);

        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(module.getTopIdentifierDeclarations().front().get());
        ASSERT_NE(classDeclaration, nullptr);
        ASSERT_EQ(classDeclaration->getConstructorDeclarations().size(), 1);

        const auto* constructor = classDeclaration->getConstructorDeclarations().front().get();
        EXPECT_EQ(constructor->getAccessModifier(), ConstructorDeclarationKind::AccessModifier::PROTECTED);
        ASSERT_EQ(constructor->getParameters().size(), 1);
        EXPECT_EQ(constructor->getParameters().front()->getName().getIdentifierString(), "value");
        EXPECT_EQ(constructor->getBody().getStatements().size(), 1);
    }

    TEST(ParserTest, OperatorDeclarationsUseOperatorDeclarationNode) {
        std::stringstream input(
            "class Vec3 {\n"
            "    protected op +(other: Vec3) -> Vec3 {\n"
            "        return this\n"
            "    }\n"
            "    op [](index: int) -> int {\n"
            "        return 0\n"
            "    }\n"
            "    override op ==(other: Vec3) -> bool {\n"
            "        return true\n"
            "    }\n"
            "}\n"
            "interface Addable {\n"
            "    metadata(deprecated) op +(other: Addable) -> Addable\n"
            "    op ()(value: int) -> int\n"
            "}\n"
        );

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 2);

        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(module.getTopIdentifierDeclarations()[0].get());
        ASSERT_NE(classDeclaration, nullptr);
        ASSERT_EQ(classDeclaration->getOperatorDeclarations().size(), 3);

        const auto* addition = classDeclaration->getOperatorDeclarations()[0].get();
        EXPECT_EQ(addition->getKind(), OperatorDeclarationKind::Kind::ADDITION);
        EXPECT_EQ(addition->getContext(), OperatorDeclarationKind::Context::CLASS);
        EXPECT_EQ(addition->getAccessModifier(), OperatorDeclarationKind::AccessModifier::PROTECTED);
        EXPECT_EQ(addition->getOperatorName(), "addition");
        ASSERT_EQ(addition->getParameters().size(), 1);
        EXPECT_EQ(addition->getParameters().front()->getName().getIdentifierString(), "other");
        EXPECT_NE(addition->getReturnType(), nullptr);
        EXPECT_NE(addition->getBody(), nullptr);

        const auto* subscript = classDeclaration->getOperatorDeclarations()[1].get();
        EXPECT_EQ(subscript->getKind(), OperatorDeclarationKind::Kind::SUBSCRIPT);
        EXPECT_EQ(subscript->getOperatorName(), "subscript");

        const auto* equality = classDeclaration->getOperatorDeclarations()[2].get();
        EXPECT_EQ(equality->getKind(), OperatorDeclarationKind::Kind::EQUAL);
        EXPECT_EQ(equality->getOperatorName(), "equality");

        const auto* interfaceDeclaration = dynamic_cast<const InterfaceDeclarationNode*>(module.getTopIdentifierDeclarations()[1].get());
        ASSERT_NE(interfaceDeclaration, nullptr);
        ASSERT_EQ(interfaceDeclaration->getOperatorDeclarations().size(), 2);

        const auto& interfaceAddition = interfaceDeclaration->getOperatorDeclarations()[0];
        EXPECT_EQ(interfaceAddition->getKind(), OperatorDeclarationKind::Kind::ADDITION);
        EXPECT_EQ(interfaceAddition->getContext(), OperatorDeclarationKind::Context::INTERFACE);
        EXPECT_TRUE(interfaceAddition->doesIncludeMetadata());
        EXPECT_EQ(interfaceAddition->getBody(), nullptr);

        const auto& call = interfaceDeclaration->getOperatorDeclarations()[1];
        EXPECT_EQ(call->getKind(), OperatorDeclarationKind::Kind::CALL);
        EXPECT_EQ(call->getOperatorName(), "call");
    }

    TEST(ParserTest, ParsesNativeOperatorDeclarationsAlongsideNativeFunctions) {
        std::stringstream input(
            "native func log(message: string)\n"
            "class Vec3 {\n"
            "    native op +(other: Vec3) -> Vec3\n"
            "    private native op [](index: int) -> int\n"
            "    override native op ==(other: Vec3) -> bool\n"
            "    metadata(deprecated) native\n"
            "    op -(other: Vec3) -> Vec3\n"
            "    op *(scale: int) -> Vec3 {\n"
            "        return this\n"
            "    }\n"
            "    native func length() -> int\n"
            "    static native func zero() -> Vec3\n"
            "}\n"
        );

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 2);

        const auto* functionDeclaration = dynamic_cast<const FunctionDeclarationNode*>(module.getTopIdentifierDeclarations()[0].get());
        ASSERT_NE(functionDeclaration, nullptr);
        EXPECT_EQ(functionDeclaration->getImplementation(), FunctionDeclarationKind::Implementation::NATIVE);
        EXPECT_EQ(functionDeclaration->getName().getIdentifierString(), "log");
        EXPECT_EQ(functionDeclaration->getBody(), nullptr);

        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(module.getTopIdentifierDeclarations()[1].get());
        ASSERT_NE(classDeclaration, nullptr);
        ASSERT_EQ(classDeclaration->getOperatorDeclarations().size(), 5);
        ASSERT_EQ(classDeclaration->getMethodDeclarations().size(), 2);

        const auto& addition = classDeclaration->getOperatorDeclarations()[0];
        EXPECT_EQ(addition->getKind(), OperatorDeclarationKind::Kind::ADDITION);
        EXPECT_EQ(addition->getImplementation(), OperatorDeclarationKind::Implementation::NATIVE);
        EXPECT_EQ(addition->getContext(), OperatorDeclarationKind::Context::CLASS);
        EXPECT_EQ(addition->getAccessModifier(), OperatorDeclarationKind::AccessModifier::PUBLIC);
        ASSERT_EQ(addition->getParameters().size(), 1);
        EXPECT_EQ(addition->getParameters().front()->getName().getIdentifierString(), "other");
        EXPECT_NE(addition->getReturnType(), nullptr);
        EXPECT_EQ(addition->getBody(), nullptr);

        const auto& subscript = classDeclaration->getOperatorDeclarations()[1];
        EXPECT_EQ(subscript->getKind(), OperatorDeclarationKind::Kind::SUBSCRIPT);
        EXPECT_EQ(subscript->getImplementation(), OperatorDeclarationKind::Implementation::NATIVE);
        EXPECT_EQ(subscript->getAccessModifier(), OperatorDeclarationKind::AccessModifier::PRIVATE);
        EXPECT_EQ(subscript->getBody(), nullptr);

        const auto& equality = classDeclaration->getOperatorDeclarations()[2];
        EXPECT_EQ(equality->getKind(), OperatorDeclarationKind::Kind::EQUAL);
        EXPECT_EQ(equality->getImplementation(), OperatorDeclarationKind::Implementation::NATIVE);
        EXPECT_EQ(equality->getBody(), nullptr);

        const auto& subtraction = classDeclaration->getOperatorDeclarations()[3];
        EXPECT_EQ(subtraction->getKind(), OperatorDeclarationKind::Kind::SUBTRACTION);
        EXPECT_EQ(subtraction->getImplementation(), OperatorDeclarationKind::Implementation::NATIVE);
        EXPECT_TRUE(subtraction->doesIncludeMetadata());
        EXPECT_EQ(subtraction->getBody(), nullptr);

        const auto& multiplication = classDeclaration->getOperatorDeclarations()[4];
        EXPECT_EQ(multiplication->getKind(), OperatorDeclarationKind::Kind::MULTIPLICATION);
        EXPECT_EQ(multiplication->getImplementation(), OperatorDeclarationKind::Implementation::REGULAR);
        EXPECT_NE(multiplication->getBody(), nullptr);

        const auto& length = classDeclaration->getMethodDeclarations()[0];
        EXPECT_EQ(length->getImplementation(), FunctionDeclarationKind::Implementation::NATIVE);
        EXPECT_EQ(length->getBinding(), FunctionDeclarationKind::Binding::INSTANCE);
        EXPECT_EQ(length->getBody(), nullptr);

        const auto& zero = classDeclaration->getMethodDeclarations()[1];
        EXPECT_EQ(zero->getImplementation(), FunctionDeclarationKind::Implementation::NATIVE);
        EXPECT_EQ(zero->getBinding(), FunctionDeclarationKind::Binding::STATIC);
        EXPECT_EQ(zero->getBody(), nullptr);
    }

    TEST(ParserTest, KeepsImplementationAccessModifierAndBindingOfFunctionDeclarations) {
        std::stringstream input(
            "func run() {}\n"
            "class Counter {\n"
            "    private static func reset() {}\n"
            "    protected override func read() -> int {\n"
            "        return 0\n"
            "    }\n"
            "    private static native func create() -> Counter\n"
            "}\n"
            "interface Readable {\n"
            "    metadata(deprecated) func read() -> int\n"
            "}\n"
        );

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 3);

        const auto* run = dynamic_cast<const FunctionDeclarationNode*>(module.getTopIdentifierDeclarations()[0].get());
        ASSERT_NE(run, nullptr);
        EXPECT_EQ(run->getImplementation(), FunctionDeclarationKind::Implementation::REGULAR);
        EXPECT_EQ(run->getContext(), FunctionDeclarationKind::Context::TOP_LEVEL);
        EXPECT_EQ(run->getBinding(), FunctionDeclarationKind::Binding::STATIC);
        EXPECT_NE(run->getBody(), nullptr);

        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(module.getTopIdentifierDeclarations()[1].get());
        ASSERT_NE(classDeclaration, nullptr);
        ASSERT_EQ(classDeclaration->getMethodDeclarations().size(), 3);

        const auto& reset = classDeclaration->getMethodDeclarations()[0];
        EXPECT_EQ(reset->getImplementation(), FunctionDeclarationKind::Implementation::REGULAR);
        EXPECT_EQ(reset->getContext(), FunctionDeclarationKind::Context::CLASS);
        EXPECT_EQ(reset->getAccessModifier(), FunctionDeclarationKind::AccessModifier::PRIVATE);
        EXPECT_EQ(reset->getBinding(), FunctionDeclarationKind::Binding::STATIC);
        EXPECT_NE(reset->getBody(), nullptr);

        const auto& read = classDeclaration->getMethodDeclarations()[1];
        EXPECT_EQ(read->getImplementation(), FunctionDeclarationKind::Implementation::REGULAR);
        EXPECT_EQ(read->getAccessModifier(), FunctionDeclarationKind::AccessModifier::PROTECTED);
        EXPECT_EQ(read->getBinding(), FunctionDeclarationKind::Binding::INSTANCE);
        EXPECT_NE(read->getBody(), nullptr);

        const auto& create = classDeclaration->getMethodDeclarations()[2];
        EXPECT_EQ(create->getImplementation(), FunctionDeclarationKind::Implementation::NATIVE);
        EXPECT_EQ(create->getAccessModifier(), FunctionDeclarationKind::AccessModifier::PRIVATE);
        EXPECT_EQ(create->getBinding(), FunctionDeclarationKind::Binding::STATIC);
        EXPECT_EQ(create->getBody(), nullptr);

        const auto* interfaceDeclaration = dynamic_cast<const InterfaceDeclarationNode*>(module.getTopIdentifierDeclarations()[2].get());
        ASSERT_NE(interfaceDeclaration, nullptr);
        ASSERT_EQ(interfaceDeclaration->getMethodDeclarations().size(), 1);

        const auto& interfaceRead = interfaceDeclaration->getMethodDeclarations().front();
        EXPECT_EQ(interfaceRead->getImplementation(), FunctionDeclarationKind::Implementation::REGULAR);
        EXPECT_EQ(interfaceRead->getContext(), FunctionDeclarationKind::Context::INTERFACE);
        EXPECT_EQ(interfaceRead->getAccessModifier(), FunctionDeclarationKind::AccessModifier::PUBLIC);
        EXPECT_EQ(interfaceRead->getBinding(), FunctionDeclarationKind::Binding::INSTANCE);
        EXPECT_TRUE(interfaceRead->doesIncludeMetadata());
        EXPECT_EQ(interfaceRead->getBody(), nullptr);
    }

    TEST(ParserTest, ParsesModulePrivateTopLevelDeclarations) {
        const std::string source = "let shown = 1\n"
                                   "private let hidden = 2\n"
                                   "func run() {}\n"
                                   "private func helper() {}\n"
                                   "private native func bridge() -> int\n"
                                   "private class Box {}\n"
                                   "private final class Sealed {}\n"
                                   "private interface Readable {}\n"
                                   "private enum State {\n"
                                   "    Ready\n"
                                   "}\n"
                                   "private type Alias = int\n"
                                   "metadata(deprecated) private let legacy = 0\n"
                                   "class Visible {}\n";
        std::stringstream input(source);

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& declarations = result.getModuleNode().getTopIdentifierDeclarations();
        ASSERT_EQ(declarations.size(), 12);

        const auto* shown = dynamic_cast<const ValueDeclarationNode*>(declarations[0].get());
        ASSERT_NE(shown, nullptr);
        EXPECT_EQ(shown->getAccessModifier(), ValueDeclarationKind::AccessModifier::PUBLIC);

        const auto* hidden = dynamic_cast<const ValueDeclarationNode*>(declarations[1].get());
        ASSERT_NE(hidden, nullptr);
        EXPECT_EQ(hidden->getAccessModifier(), ValueDeclarationKind::AccessModifier::PRIVATE);
        EXPECT_EQ(hidden->getOffset(), source.find("private let hidden"));

        const auto* run = dynamic_cast<const FunctionDeclarationNode*>(declarations[2].get());
        ASSERT_NE(run, nullptr);
        EXPECT_EQ(run->getAccessModifier(), FunctionDeclarationKind::AccessModifier::PUBLIC);

        const auto* helper = dynamic_cast<const FunctionDeclarationNode*>(declarations[3].get());
        ASSERT_NE(helper, nullptr);
        EXPECT_EQ(helper->getAccessModifier(), FunctionDeclarationKind::AccessModifier::PRIVATE);
        EXPECT_EQ(helper->getContext(), FunctionDeclarationKind::Context::TOP_LEVEL);
        EXPECT_EQ(helper->getOffset(), source.find("private func helper"));

        const auto* bridge = dynamic_cast<const FunctionDeclarationNode*>(declarations[4].get());
        ASSERT_NE(bridge, nullptr);
        EXPECT_EQ(bridge->getAccessModifier(), FunctionDeclarationKind::AccessModifier::PRIVATE);
        EXPECT_EQ(bridge->getImplementation(), FunctionDeclarationKind::Implementation::NATIVE);
        EXPECT_EQ(bridge->getBody(), nullptr);

        const auto* box = dynamic_cast<const ClassDeclarationNode*>(declarations[5].get());
        ASSERT_NE(box, nullptr);
        EXPECT_EQ(box->getAccessModifier(), TypeDeclarationKind::AccessModifier::PRIVATE);
        EXPECT_FALSE(box->isFinal());
        EXPECT_EQ(box->getOffset(), source.find("private class Box"));

        const auto* sealed = dynamic_cast<const ClassDeclarationNode*>(declarations[6].get());
        ASSERT_NE(sealed, nullptr);
        EXPECT_EQ(sealed->getAccessModifier(), TypeDeclarationKind::AccessModifier::PRIVATE);
        EXPECT_TRUE(sealed->isFinal());

        const auto* readable = dynamic_cast<const InterfaceDeclarationNode*>(declarations[7].get());
        ASSERT_NE(readable, nullptr);
        EXPECT_EQ(readable->getAccessModifier(), TypeDeclarationKind::AccessModifier::PRIVATE);

        const auto* state = dynamic_cast<const EnumDeclarationNode*>(declarations[8].get());
        ASSERT_NE(state, nullptr);
        EXPECT_EQ(state->getAccessModifier(), TypeDeclarationKind::AccessModifier::PRIVATE);
        EXPECT_EQ(state->getMemberDeclarations().size(), 1);

        const auto* alias = dynamic_cast<const TypeAliasDeclarationNode*>(declarations[9].get());
        ASSERT_NE(alias, nullptr);
        EXPECT_EQ(alias->getAccessModifier(), TypeDeclarationKind::AccessModifier::PRIVATE);

        const auto* legacy = dynamic_cast<const ValueDeclarationNode*>(declarations[10].get());
        ASSERT_NE(legacy, nullptr);
        EXPECT_EQ(legacy->getAccessModifier(), ValueDeclarationKind::AccessModifier::PRIVATE);
        EXPECT_TRUE(legacy->doesIncludeMetadata());
        EXPECT_EQ(legacy->getOffset(), source.find("metadata(deprecated)"));

        const auto* visible = dynamic_cast<const ClassDeclarationNode*>(declarations[11].get());
        ASSERT_NE(visible, nullptr);
        EXPECT_EQ(visible->getAccessModifier(), TypeDeclarationKind::AccessModifier::PUBLIC);
    }

    TEST(ParserTest, RejectsExportDeclarationsAndOtherTopLevelAccessModifiers) {
        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        for (const auto source : {
                 "class Box {}\nexport Box\n",
                 "export class Box {}\n",
                 "let export = 0\n",
                 "public let value = 0\n",
                 "protected func run() {}\n",
                 "private private let value = 0\n",
                 "private metadata(deprecated) let value = 0\n",
             }) {
            SCOPED_TRACE(source);
            std::stringstream input(source);

            EXPECT_THROW(static_cast<void>(parseModule(input, config)), SyntaxError);
        }
    }

    TEST(ParserTest, RejectsStaticNativeOperatorDeclarations) {
        std::stringstream input(
            "class Vec3 {\n"
            "    static native op +(other: Vec3) -> Vec3\n"
            "}\n"
        );

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        EXPECT_THROW(static_cast<void>(parseModule(input, config)), SyntaxError);
    }

    TEST(ParserTest, ParsesComparisonOperatorDeclarations) {
        std::stringstream input(
            "class Vec3 {\n"
            "    op <=>(other: Vec3) -> Ordering? {\n"
            "        return none\n"
            "    }\n"
            "    native op <=>(other: int) -> Ordering?\n"
            "}\n"
            "interface Comparable {\n"
            "    op <=>(other: Comparable) -> Ordering?\n"
            "}\n"
        );

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 2);

        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(module.getTopIdentifierDeclarations()[0].get());
        ASSERT_NE(classDeclaration, nullptr);
        ASSERT_EQ(classDeclaration->getOperatorDeclarations().size(), 2);

        const auto& regularComparison = classDeclaration->getOperatorDeclarations()[0];
        EXPECT_EQ(regularComparison->getKind(), OperatorDeclarationKind::Kind::COMPARISON);
        EXPECT_EQ(regularComparison->getImplementation(), OperatorDeclarationKind::Implementation::REGULAR);
        EXPECT_EQ(regularComparison->getOperatorName(), "comparison");
        EXPECT_NE(regularComparison->getBody(), nullptr);

        const auto& nativeComparison = classDeclaration->getOperatorDeclarations()[1];
        EXPECT_EQ(nativeComparison->getKind(), OperatorDeclarationKind::Kind::COMPARISON);
        EXPECT_EQ(nativeComparison->getImplementation(), OperatorDeclarationKind::Implementation::NATIVE);
        EXPECT_EQ(nativeComparison->getBody(), nullptr);

        const auto* interfaceDeclaration = dynamic_cast<const InterfaceDeclarationNode*>(module.getTopIdentifierDeclarations()[1].get());
        ASSERT_NE(interfaceDeclaration, nullptr);
        ASSERT_EQ(interfaceDeclaration->getOperatorDeclarations().size(), 1);
        EXPECT_EQ(interfaceDeclaration->getOperatorDeclarations().front()->getKind(), OperatorDeclarationKind::Kind::COMPARISON);
        EXPECT_EQ(interfaceDeclaration->getOperatorDeclarations().front()->getContext(), OperatorDeclarationKind::Context::INTERFACE);
    }

    TEST(ParserTest, RejectsOverloadingOfDerivedComparisonOperators) {
        constexpr std::string_view operators[] = { "!=", "<", ">", "<=", ">=" };

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        for (const auto operatorSymbol : operators) {
            std::stringstream input("class Vec3 {\n    op " + std::string(operatorSymbol) + "(other: Vec3) -> bool {\n        return true\n    }\n}\n");

            EXPECT_THROW(static_cast<void>(parseModule(input, config)), SyntaxError) << "op " << operatorSymbol;
        }
    }

    TEST(ParserTest, DistributesClassMembersIntoDeclarationKinds) {
        std::stringstream input(
            "class Example {\n"
            "    value: int\n"
            "    static count: int = 0\n"
            "    func reset() {}\n"
            "    init() {}\n"
            "    op +(other: Example) -> Example {\n"
            "        return this\n"
            "    }\n"
            "}\n"
        );

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 1);

        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(module.getTopIdentifierDeclarations().front().get());
        ASSERT_NE(classDeclaration, nullptr);
        EXPECT_EQ(classDeclaration->getPropertyDeclarations().size(), 2);
        EXPECT_EQ(classDeclaration->getMethodDeclarations().size(), 1);
        EXPECT_EQ(classDeclaration->getConstructorDeclarations().size(), 1);
        EXPECT_EQ(classDeclaration->getOperatorDeclarations().size(), 1);
    }

    TEST(ParserTest, ParsesNoneAsPrimaryExpression) {
        std::stringstream input("func test() {\n    let value = none\n    let range = 1..none\n}\n");

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 1);

        const auto* function = dynamic_cast<const FunctionDeclarationNode*>(module.getTopIdentifierDeclarations().front().get());
        ASSERT_NE(function, nullptr);
        ASSERT_NE(function->getBody(), nullptr);

        const auto& statements = function->getBody()->getStatements();
        ASSERT_EQ(statements.size(), 2);

        const auto* valueStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[0].get());
        ASSERT_NE(valueStatement, nullptr);
        ASSERT_NE(valueStatement->getVariableDeclaration().getInitializer(), nullptr);
        EXPECT_NE(dynamic_cast<const NoneExpressionNode*>(valueStatement->getVariableDeclaration().getInitializer()), nullptr);

        const auto* rangeStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[1].get());
        ASSERT_NE(rangeStatement, nullptr);
        ASSERT_NE(rangeStatement->getVariableDeclaration().getInitializer(), nullptr);

        const auto* rangeExpression = dynamic_cast<const RangeExpressionNode*>(rangeStatement->getVariableDeclaration().getInitializer());
        ASSERT_NE(rangeExpression, nullptr);
        ASSERT_NE(rangeExpression->getEnd(), nullptr);
        EXPECT_NE(dynamic_cast<const NoneExpressionNode*>(rangeExpression->getEnd()), nullptr);
    }

    TEST(ParserTest, DistinguishesPrimitiveAndCustomizedTypeAnnotations) {
        std::stringstream input(
            "class Example {}\n"
            "func test() {\n"
            "    let primitiveType = int\n"
            "    let identifier = value\n"
            "    let primitiveValue: int? = none\n"
            "    let customizedValue: Example = none\n"
            "}\n"
        );

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 2);

        const auto* function = dynamic_cast<const FunctionDeclarationNode*>(module.getTopIdentifierDeclarations()[1].get());
        ASSERT_NE(function, nullptr);
        ASSERT_NE(function->getBody(), nullptr);

        const auto& statements = function->getBody()->getStatements();
        ASSERT_EQ(statements.size(), 4);

        const auto* primitiveExpressionStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[0].get());
        ASSERT_NE(primitiveExpressionStatement, nullptr);
        ASSERT_NE(primitiveExpressionStatement->getVariableDeclaration().getInitializer(), nullptr);
        const auto* primitiveExpression = dynamic_cast<const PrimitiveTypeExpressionNode*>(primitiveExpressionStatement->getVariableDeclaration().getInitializer());
        ASSERT_NE(primitiveExpression, nullptr);
        EXPECT_EQ(primitiveExpression->getKind(), PrimitiveTypeExpressionKind::INT);

        const auto* identifierStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[1].get());
        ASSERT_NE(identifierStatement, nullptr);
        ASSERT_NE(identifierStatement->getVariableDeclaration().getInitializer(), nullptr);
        EXPECT_NE(dynamic_cast<const IdentifierLikeExpressionNode*>(identifierStatement->getVariableDeclaration().getInitializer()), nullptr);

        const auto* primitiveValueStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[2].get());
        ASSERT_NE(primitiveValueStatement, nullptr);
        ASSERT_NE(primitiveValueStatement->getVariableDeclaration().getType(), nullptr);
        const auto* primitiveReference = dynamic_cast<const PrimitiveTypeAnnotationNode*>(primitiveValueStatement->getVariableDeclaration().getType());
        ASSERT_NE(primitiveReference, nullptr);
        EXPECT_EQ(primitiveReference->getKind(), PrimitiveTypeAnnotationKind::INT);
        EXPECT_EQ(primitiveReference->getPrimitiveTypeName(), "int");
        EXPECT_TRUE(primitiveReference->hasQuestionMarkSuffix());

        const auto* customizedValueStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[3].get());
        ASSERT_NE(customizedValueStatement, nullptr);
        ASSERT_NE(customizedValueStatement->getVariableDeclaration().getType(), nullptr);
        const auto* customizedReference = dynamic_cast<const CustomizedTypeAnnotationNode*>(customizedValueStatement->getVariableDeclaration().getType());
        ASSERT_NE(customizedReference, nullptr);
        ASSERT_EQ(customizedReference->getNameParts().size(), 1);
        EXPECT_EQ(customizedReference->getNameParts().front()->getIdentifierString(), "Example");
        EXPECT_FALSE(customizedReference->hasQuestionMarkSuffix());
        EXPECT_TRUE(customizedReference->getGenericArguments().empty());
    }

    TEST(ParserTest, ParsesGenericArgumentsForLocalTypeDeclarationsOnly) {
        std::stringstream input(
            "class Box {}\n"
            "class Pair {}\n"
            "class Item {}\n"
            "func test() {\n"
            "    let generic = Box<Pair<Item>>\n"
            "    let qualified = Box<models.Item>\n"
            "    let nullable = Box<int?>\n"
            "    let compared = left < right\n"
            "}\n"
        );

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 4);

        const auto* function = dynamic_cast<const FunctionDeclarationNode*>(module.getTopIdentifierDeclarations()[3].get());
        ASSERT_NE(function, nullptr);
        ASSERT_NE(function->getBody(), nullptr);

        const auto& statements = function->getBody()->getStatements();
        ASSERT_EQ(statements.size(), 4);

        const auto* genericStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[0].get());
        ASSERT_NE(genericStatement, nullptr);
        ASSERT_NE(genericStatement->getVariableDeclaration().getInitializer(), nullptr);
        const auto* genericExpression = dynamic_cast<const IdentifierLikeExpressionNode*>(genericStatement->getVariableDeclaration().getInitializer());
        ASSERT_NE(genericExpression, nullptr);
        EXPECT_EQ(genericExpression->getName().getIdentifierString(), "Box");
        ASSERT_EQ(genericExpression->getGenericArguments().size(), 1);
        const auto* pairReference = dynamic_cast<const CustomizedTypeAnnotationNode*>(genericExpression->getGenericArguments().front().get());
        ASSERT_NE(pairReference, nullptr);
        ASSERT_EQ(pairReference->getNameParts().size(), 1);
        EXPECT_EQ(pairReference->getNameParts().front()->getIdentifierString(), "Pair");
        ASSERT_EQ(pairReference->getGenericArguments().size(), 1);
        const auto* itemReference = dynamic_cast<const CustomizedTypeAnnotationNode*>(pairReference->getGenericArguments().front().get());
        ASSERT_NE(itemReference, nullptr);
        ASSERT_EQ(itemReference->getNameParts().size(), 1);
        EXPECT_EQ(itemReference->getNameParts().front()->getIdentifierString(), "Item");
        EXPECT_TRUE(itemReference->getGenericArguments().empty());

        const auto* qualifiedStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[1].get());
        ASSERT_NE(qualifiedStatement, nullptr);
        ASSERT_NE(qualifiedStatement->getVariableDeclaration().getInitializer(), nullptr);
        const auto* qualifiedExpression = dynamic_cast<const IdentifierLikeExpressionNode*>(qualifiedStatement->getVariableDeclaration().getInitializer());
        ASSERT_NE(qualifiedExpression, nullptr);
        ASSERT_EQ(qualifiedExpression->getGenericArguments().size(), 1);
        const auto* qualifiedReference = dynamic_cast<const CustomizedTypeAnnotationNode*>(qualifiedExpression->getGenericArguments().front().get());
        ASSERT_NE(qualifiedReference, nullptr);
        ASSERT_EQ(qualifiedReference->getNameParts().size(), 2);
        EXPECT_EQ(qualifiedReference->getNameParts()[0]->getIdentifierString(), "models");
        EXPECT_EQ(qualifiedReference->getNameParts()[1]->getIdentifierString(), "Item");

        const auto* nullableStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[2].get());
        ASSERT_NE(nullableStatement, nullptr);
        ASSERT_NE(nullableStatement->getVariableDeclaration().getInitializer(), nullptr);
        const auto* nullableExpression = dynamic_cast<const IdentifierLikeExpressionNode*>(nullableStatement->getVariableDeclaration().getInitializer());
        ASSERT_NE(nullableExpression, nullptr);
        ASSERT_EQ(nullableExpression->getGenericArguments().size(), 1);
        const auto* nullableReference = dynamic_cast<const PrimitiveTypeAnnotationNode*>(nullableExpression->getGenericArguments().front().get());
        ASSERT_NE(nullableReference, nullptr);
        EXPECT_EQ(nullableReference->getKind(), PrimitiveTypeAnnotationKind::INT);
        EXPECT_TRUE(nullableReference->hasQuestionMarkSuffix());

        const auto* comparedStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[3].get());
        ASSERT_NE(comparedStatement, nullptr);
        ASSERT_NE(comparedStatement->getVariableDeclaration().getInitializer(), nullptr);
        const auto* comparedExpression = dynamic_cast<const BinaryExpressionNode*>(comparedStatement->getVariableDeclaration().getInitializer());
        ASSERT_NE(comparedExpression, nullptr);
        EXPECT_EQ(comparedExpression->getKind(), BinaryExpressionKind::LESS_THAN);
        EXPECT_NE(dynamic_cast<const IdentifierLikeExpressionNode*>(&comparedExpression->getLeft()), nullptr);
        EXPECT_NE(dynamic_cast<const IdentifierLikeExpressionNode*>(&comparedExpression->getRight()), nullptr);
    }

    TEST(ParserTest, RejectsNonTypeGenericArgumentsInExpressions) {
        constexpr std::string_view arguments[] = { "create()", "1", "items[0]", "this" };

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testPackageRootPath(),
            .inputFilePath = testPackageRootPath() / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        for (const auto argument : arguments) {
            std::stringstream input("class Box {}\nfunc test() {\n    let value = Box<" + std::string(argument) + ">\n}\n");

            EXPECT_THROW(static_cast<void>(parseModule(input, config)), SyntaxError) << "Box<" << argument << ">";
        }
    }

    TEST(ParserTest, ParsesGenericArgumentsForImportedTypeDeclarations) {
        const auto* testInfo = testing::UnitTest::GetInstance()->current_test_info();
        auto testDirectory = std::filesystem::temp_directory_path() / ("vnlctest-parser-" + std::string(testInfo->name()));
        std::filesystem::remove_all(testDirectory);
        std::filesystem::create_directories(testDirectory / "first_source");
        std::filesystem::create_directories(testDirectory / "second_source");

        std::ofstream firstModuleInterface(testDirectory / "first_source" / "models.vni");
        firstModuleInterface
            << R"({"Box":{"category":"class","accessModifier":"public","genericParameters":["T"],"properties":{},"methods":{},"constructors":{},"operators":{},"baseClass":null,"implementedInterfaces":[],"final":false}})";
        firstModuleInterface.close();

        std::ofstream secondModuleInterface(testDirectory / "second_source" / "models.vni");
        secondModuleInterface << R"({"Box":{"category":"interface","accessModifier":"public","genericParameters":["T"],"methods":{},"operators":{}}})";
        secondModuleInterface.close();

        std::stringstream input(
            "import first.models as firstModels\n"
            "import second.models as secondModels\n"
            "import first.models.Box as DirectBox\n"
            "func test() {\n"
            "    let first = firstModels.Box<Item>\n"
            "    let second = secondModels.Box<Value>\n"
            "    let direct = DirectBox<Member>\n"
            "}\n"
        );

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testDirectory / "source_package",
            .inputFilePath = testDirectory / "source_package" / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {
                { "first", testDirectory / "first_source" },
                { "second", testDirectory / "second_source" },
            },
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 1);

        const auto* function = dynamic_cast<const FunctionDeclarationNode*>(module.getTopIdentifierDeclarations().front().get());
        ASSERT_NE(function, nullptr);
        ASSERT_NE(function->getBody(), nullptr);

        const auto& statements = function->getBody()->getStatements();
        ASSERT_EQ(statements.size(), 3);

        const auto firstStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[0].get());
        ASSERT_NE(firstStatement, nullptr);
        ASSERT_NE(firstStatement->getVariableDeclaration().getInitializer(), nullptr);
        const auto* firstExpression = dynamic_cast<const MemberAccessExpressionNode*>(firstStatement->getVariableDeclaration().getInitializer());
        ASSERT_NE(firstExpression, nullptr);
        EXPECT_EQ(firstExpression->getMember().getName().getIdentifierString(), "Box");
        ASSERT_EQ(firstExpression->getMember().getGenericArguments().size(), 1);
        const auto* firstArgument = dynamic_cast<const CustomizedTypeAnnotationNode*>(firstExpression->getMember().getGenericArguments().front().get());
        ASSERT_NE(firstArgument, nullptr);
        ASSERT_EQ(firstArgument->getNameParts().size(), 1);
        EXPECT_EQ(firstArgument->getNameParts().front()->getIdentifierString(), "Item");

        const auto secondStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[1].get());
        ASSERT_NE(secondStatement, nullptr);
        ASSERT_NE(secondStatement->getVariableDeclaration().getInitializer(), nullptr);
        const auto* secondExpression = dynamic_cast<const MemberAccessExpressionNode*>(secondStatement->getVariableDeclaration().getInitializer());
        ASSERT_NE(secondExpression, nullptr);
        EXPECT_EQ(secondExpression->getMember().getName().getIdentifierString(), "Box");
        ASSERT_EQ(secondExpression->getMember().getGenericArguments().size(), 1);
        const auto* secondArgument = dynamic_cast<const CustomizedTypeAnnotationNode*>(secondExpression->getMember().getGenericArguments().front().get());
        ASSERT_NE(secondArgument, nullptr);
        ASSERT_EQ(secondArgument->getNameParts().size(), 1);
        EXPECT_EQ(secondArgument->getNameParts().front()->getIdentifierString(), "Value");

        const auto directStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[2].get());
        ASSERT_NE(directStatement, nullptr);
        ASSERT_NE(directStatement->getVariableDeclaration().getInitializer(), nullptr);
        const auto* directExpression = dynamic_cast<const IdentifierLikeExpressionNode*>(directStatement->getVariableDeclaration().getInitializer());
        ASSERT_NE(directExpression, nullptr);
        EXPECT_EQ(directExpression->getName().getIdentifierString(), "DirectBox");
        ASSERT_EQ(directExpression->getGenericArguments().size(), 1);
        const auto* directArgument = dynamic_cast<const CustomizedTypeAnnotationNode*>(directExpression->getGenericArguments().front().get());
        ASSERT_NE(directArgument, nullptr);
        ASSERT_EQ(directArgument->getNameParts().size(), 1);
        EXPECT_EQ(directArgument->getNameParts().front()->getIdentifierString(), "Member");

        std::error_code error;
        std::filesystem::remove_all(testDirectory, error);
    }

    TEST(ParserTest, IgnoresModulePrivateWildcardImportsWhenParsingGenericArguments) {
        const auto* testInfo = testing::UnitTest::GetInstance()->current_test_info();
        auto testDirectory = std::filesystem::temp_directory_path() / ("vnlctest-parser-" + std::string(testInfo->name()));
        std::filesystem::remove_all(testDirectory);
        std::filesystem::create_directories(testDirectory / "first_source");

        std::ofstream moduleInterface(testDirectory / "first_source" / "models.vni");
        moduleInterface << R"({
"Box":{"category":"class","accessModifier":"private","genericParameters":["T"],"properties":{},"methods":{},"constructors":{},"operators":{},"baseClass":null,"implementedInterfaces":[],"final":false},
"Pair":{"category":"class","accessModifier":"public","genericParameters":["T"],"properties":{},"methods":{},"constructors":{},"operators":{},"baseClass":null,"implementedInterfaces":[],"final":false}
})";
        moduleInterface.close();

        std::stringstream input(
            "import first.models.*\n"
            "func test() {\n"
            "    let compared = Box < left\n"
            "    let generic = Pair<Item>\n"
            "}\n"
        );

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testDirectory / "source_package",
            .inputFilePath = testDirectory / "source_package" / "test.vnl",
            .outputDirectory = {},
            .dependencyPackageRootPaths = {
                { "first", testDirectory / "first_source" },
            },
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 1);

        const auto* function = dynamic_cast<const FunctionDeclarationNode*>(module.getTopIdentifierDeclarations().front().get());
        ASSERT_NE(function, nullptr);
        ASSERT_NE(function->getBody(), nullptr);

        const auto& statements = function->getBody()->getStatements();
        ASSERT_EQ(statements.size(), 2);

        const auto* comparedStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[0].get());
        ASSERT_NE(comparedStatement, nullptr);
        const auto* comparedExpression = dynamic_cast<const BinaryExpressionNode*>(comparedStatement->getVariableDeclaration().getInitializer());
        ASSERT_NE(comparedExpression, nullptr);
        EXPECT_EQ(comparedExpression->getKind(), BinaryExpressionKind::LESS_THAN);

        const auto* genericStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[1].get());
        ASSERT_NE(genericStatement, nullptr);
        const auto* genericExpression = dynamic_cast<const IdentifierLikeExpressionNode*>(genericStatement->getVariableDeclaration().getInitializer());
        ASSERT_NE(genericExpression, nullptr);
        EXPECT_EQ(genericExpression->getName().getIdentifierString(), "Pair");
        EXPECT_EQ(genericExpression->getGenericArguments().size(), 1);

        std::error_code error;
        std::filesystem::remove_all(testDirectory, error);
    }

} // namespace vnlc
