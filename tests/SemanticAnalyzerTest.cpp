#include "semantic/SemanticAnalyzer.hpp"
#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/module/ModuleNode.hpp"
#include "ast/typeref/CustomizedTypeReferenceNode.hpp"
#include "collector/CollectionResult.hpp"
#include "collector/Collector.hpp"
#include "config/Config.hpp"
#include "inheritance/InheritanceNode.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "semantic/SemanticContext.hpp"
#include "semantic/SemanticResult.hpp"
#include "type/CustomizedTypeKind.hpp"
#include "type/PrimitiveType.hpp"
#include "vni/import/ImportedRootPackageMap.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace vnlc {

    namespace {

        Config makeConfig(std::string_view moduleFileName) {
            const auto packageRootPath = std::filesystem::current_path() / "semantic_test_package";

            return Config{
                .mode = RunningMode::COMPILE,
                .vanillangVersion = "1.0",
                .minecraftVersion = "26.1.2",
                .packageRootPath = packageRootPath,
                .inputFilePath = packageRootPath / "models" / moduleFileName,
                .outputDirectory = {},
                .moduleInterfaceOutputDirectory = {},
                .dependencyPackageRootPaths = {},
                .optimizationLevel = std::nullopt,
            };
        }

        std::shared_ptr<const ModuleNode> parseModule(std::string_view source, const Config& config) {
            std::stringstream input{ std::string(source) };
            Lexer collectorLexer(input);
            Collector collector(std::move(collectorLexer));
            auto collectionResult = collector.collect(config);

            input.clear();
            input.seekg(0);

            Lexer lexer(input);
            Parser parser(std::move(lexer), collectionResult);
            auto parseResult = std::make_shared<ParseResult>(parser.parse(config));
            return std::shared_ptr<const ModuleNode>(parseResult, &parseResult->getModuleNode());
        }

        CollectionResult collectModule(std::string_view source, const Config& config) {
            std::stringstream input{ std::string(source) };
            Lexer lexer(input);
            Collector collector(std::move(lexer));
            return collector.collect(config);
        }

        const RegularSymbol* asRegularSymbol(const Symbol* symbol) {
            return dynamic_cast<const RegularSymbol*>(symbol);
        }

    } // namespace

    TEST(SemanticAnalyzerTest, AcceptsMatchingGenericArgumentCounts) {
        constexpr std::string_view source = R"(
class Box<T> {}
type BoxAlias = Box<int>
type Identity<T> = T
)";
        const auto config = makeConfig("generic_arguments.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_FALSE(result.hasErrors());
    }

    TEST(SemanticAnalyzerTest, SubstitutesGenericArgumentsThroughLocalAliasTypeNames) {
        constexpr std::string_view source = R"(
class Box<T> {}
type Identity<T> = T
type Boxed<T> = Box<Identity<T>>
type Result = Boxed<int>
)";
        const auto config = makeConfig("local_alias_type_names.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_FALSE(result.hasErrors());
        const auto& resultAlias = dynamic_cast<const TypeAliasDeclarationNode&>(*module->getTopIdentifierDeclarations()[3]);
        const auto* customizedType = dynamic_cast<const CustomizedType*>(result.getTypeByTypeReferenceNode(&resultAlias.getOriginalType()));
        ASSERT_NE(customizedType, nullptr);
        EXPECT_EQ(customizedType->getFullTypeName(), std::string(module->getFullName()) + ".Box<int>");
        ASSERT_EQ(customizedType->getGenericArguments().size(), 1);
        EXPECT_EQ(customizedType->getGenericArguments().front(), PrimitiveType::intType());
        EXPECT_EQ(customizedType->getLocalNode(), module->getTopIdentifierDeclarations().front().get());
    }

    TEST(SemanticAnalyzerTest, SubstitutesAliasesInsideGenericArgumentTypeNames) {
        constexpr std::string_view source = R"(
class Box<T> {}
class Inner<T> {}
type Alias<T> = Inner<T>
type Result = Box<Alias<int>>
)";
        const auto config = makeConfig("nested_alias_type_names.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_FALSE(result.hasErrors());
        const auto& resultAlias = dynamic_cast<const TypeAliasDeclarationNode&>(*module->getTopIdentifierDeclarations()[3]);
        const auto* customizedType = dynamic_cast<const CustomizedType*>(result.getTypeByTypeReferenceNode(&resultAlias.getOriginalType()));
        ASSERT_NE(customizedType, nullptr);
        EXPECT_EQ(customizedType->getFullTypeName(), std::string(module->getFullName()) + ".Box<" + std::string(module->getFullName()) + ".Inner<int>>");
        ASSERT_EQ(customizedType->getGenericArguments().size(), 1);
        const auto* innerType = dynamic_cast<const CustomizedType*>(customizedType->getGenericArguments().front());
        ASSERT_NE(innerType, nullptr);
        EXPECT_EQ(innerType->getFullTypeName(), std::string(module->getFullName()) + ".Inner<int>");
        ASSERT_EQ(innerType->getGenericArguments().size(), 1);
        EXPECT_EQ(innerType->getGenericArguments().front(), PrimitiveType::intType());
    }

    TEST(SemanticAnalyzerTest, ResolvesNestedAliasArgumentsToTheFinalTypeNode) {
        constexpr std::string_view source = R"(
class Bar<K, V> {}
type Foo<T> = Bar<T, int>
type Baz<U> = Foo<U>
type Result = Baz<float>
)";
        const auto config = makeConfig("nested_alias_arguments.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_FALSE(result.hasErrors());
        const auto& resultAlias = dynamic_cast<const TypeAliasDeclarationNode&>(*module->getTopIdentifierDeclarations()[3]);
        const auto* customizedType = dynamic_cast<const CustomizedType*>(result.getTypeByTypeReferenceNode(&resultAlias.getOriginalType()));
        ASSERT_NE(customizedType, nullptr);
        EXPECT_EQ(customizedType->getFullTypeName(), std::string(module->getFullName()) + ".Bar<float, int>");
        ASSERT_EQ(customizedType->getGenericArguments().size(), 2);
        EXPECT_EQ(customizedType->getGenericArguments()[0], PrimitiveType::floatType());
        EXPECT_EQ(customizedType->getGenericArguments()[1], PrimitiveType::intType());
        EXPECT_EQ(customizedType->getLocalNode(), module->getTopIdentifierDeclarations().front().get());
        EXPECT_EQ(result.getCustomizedTypeByFullTypeName(std::string(module->getFullName()) + ".Bar<float, int>"), customizedType);
    }

    TEST(SemanticAnalyzerTest, RejectsGenericArgumentCountMismatchWithoutCheckingArguments) {
        constexpr std::string_view source = R"(
class Box<T> {}
type MissingArgument = Box
type ExtraArgument = Box<Missing, Missing>
)";
        const auto config = makeConfig("generic_argument_errors.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_TRUE(result.hasErrors());
        ASSERT_EQ(result.getErrors().size(), 2);
        EXPECT_EQ(result.getErrors()[0].getMessage(), "Generic argument count mismatch: expected 1, got 0");
        EXPECT_EQ(result.getErrors()[1].getMessage(), "Generic argument count mismatch: expected 1, got 2");
    }

    TEST(SemanticAnalyzerTest, DoesNotCheckGenericArgumentsOfInvalidTypeReferences) {
        constexpr std::string_view source = R"(
type Invalid = Missing<Unknown>
)";
        const auto config = makeConfig("invalid_generic_type.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_TRUE(result.hasErrors());
        ASSERT_EQ(result.getErrors().size(), 1);
        EXPECT_EQ(result.getErrors().front().getMessage(), "Use of undeclared type 'Missing'");
    }

    TEST(SemanticAnalyzerTest, AcceptsPrimitiveTypeExpressions) {
        constexpr std::string_view source = R"(
func test() {
    let value = int
}
)";
        const auto config = makeConfig("primitive_type_expression.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_FALSE(result.hasErrors());
    }

    TEST(SemanticAnalyzerTest, RejectsDuplicateOperatorParameterDeclarations) {
        constexpr std::string_view source = R"(
class Vec3 {
    op +(value: int, value: int) -> Vec3 {
        return this
    }
}
)";
        const auto config = makeConfig("operator_errors.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_TRUE(result.hasErrors());
        ASSERT_EQ(result.getErrors().size(), 1);
        EXPECT_EQ(result.getErrors()[0].getMessage(), "Redeclaration of parameter 'value'");
    }

    TEST(SemanticAnalyzerTest, GeneratesInternalNamesForParameterlessCallables) {
        constexpr std::string_view source = R"(
func reset() {}
class Counter {
    func reset() {}
    init() {}
    op ()() -> int { return 0 }
}
interface Resettable {
    func reset()
    op ()() -> int
}
)";
        const auto config = makeConfig("parameterless_internal_names.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_FALSE(result.hasErrors());
        const auto& declarations = module->getTopIdentifierDeclarations();
        ASSERT_EQ(declarations.size(), 3);
        const auto* functionDeclaration = dynamic_cast<const FunctionDeclarationNode*>(declarations[0].get());
        ASSERT_NE(functionDeclaration, nullptr);
        EXPECT_EQ(functionDeclaration->getInternalName(), "vnl-function-reset()");

        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(declarations[1].get());
        ASSERT_NE(classDeclaration, nullptr);
        ASSERT_EQ(classDeclaration->getMethodDeclarations().size(), 1);
        ASSERT_EQ(classDeclaration->getConstructorDeclarations().size(), 1);
        ASSERT_EQ(classDeclaration->getOperatorDeclarations().size(), 1);
        EXPECT_EQ(classDeclaration->getMethodDeclarations().front()->getInternalName(), "vnl-function-reset()");
        EXPECT_EQ(classDeclaration->getConstructorDeclarations().front()->getInternalName(), "vnl-constructor()");
        EXPECT_EQ(classDeclaration->getOperatorDeclarations().front()->getInternalName(), "vnl-operator-call()");

        const auto* interfaceDeclaration = dynamic_cast<const InterfaceDeclarationNode*>(declarations[2].get());
        ASSERT_NE(interfaceDeclaration, nullptr);
        ASSERT_EQ(interfaceDeclaration->getMethodDeclarations().size(), 1);
        ASSERT_EQ(interfaceDeclaration->getOperatorDeclarations().size(), 1);
        EXPECT_EQ(interfaceDeclaration->getMethodDeclarations().front()->getInternalName(), "vnl-function-reset()");
        EXPECT_EQ(interfaceDeclaration->getOperatorDeclarations().front()->getInternalName(), "vnl-operator-call()");
    }

    TEST(SemanticAnalyzerTest, GeneratesInternalNamesFromResolvedParameterTypes) {
        constexpr std::string_view source = R"(
func combine(first: Alias, second: Box<int>, third: string) {}
class Holder {
    func combine(first: Alias, second: Box<int>, third: string) {}
    init(first: Alias, second: Box<int>, third: string) {}
    op ()(first: Alias, second: Box<int>, third: string) -> int { return 0 }
}
interface Combiner {
    func combine(first: Alias, second: Box<int>, third: string)
    op ()(first: Alias, second: Box<int>, third: string) -> int
}
type Alias = Later
class Box<T> {}
class Later {}
)";
        const auto config = makeConfig("resolved_parameter_internal_names.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_FALSE(result.hasErrors());
        const auto& declarations = module->getTopIdentifierDeclarations();
        ASSERT_EQ(declarations.size(), 6);
        const std::string signature = "(" + std::string(module->getFullName()) + ".Later, " + std::string(module->getFullName()) + ".Box<int>, string)";
        const auto* functionDeclaration = dynamic_cast<const FunctionDeclarationNode*>(declarations[0].get());
        ASSERT_NE(functionDeclaration, nullptr);
        EXPECT_EQ(functionDeclaration->getInternalName(), "vnl-function-combine" + signature);

        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(declarations[1].get());
        ASSERT_NE(classDeclaration, nullptr);
        ASSERT_EQ(classDeclaration->getMethodDeclarations().size(), 1);
        ASSERT_EQ(classDeclaration->getConstructorDeclarations().size(), 1);
        ASSERT_EQ(classDeclaration->getOperatorDeclarations().size(), 1);
        EXPECT_EQ(classDeclaration->getMethodDeclarations().front()->getInternalName(), "vnl-function-combine" + signature);
        EXPECT_EQ(classDeclaration->getConstructorDeclarations().front()->getInternalName(), "vnl-constructor" + signature);
        EXPECT_EQ(classDeclaration->getOperatorDeclarations().front()->getInternalName(), "vnl-operator-call" + signature);

        const auto* interfaceDeclaration = dynamic_cast<const InterfaceDeclarationNode*>(declarations[2].get());
        ASSERT_NE(interfaceDeclaration, nullptr);
        ASSERT_EQ(interfaceDeclaration->getMethodDeclarations().size(), 1);
        ASSERT_EQ(interfaceDeclaration->getOperatorDeclarations().size(), 1);
        EXPECT_EQ(interfaceDeclaration->getMethodDeclarations().front()->getInternalName(), "vnl-function-combine" + signature);
        EXPECT_EQ(interfaceDeclaration->getOperatorDeclarations().front()->getInternalName(), "vnl-operator-call" + signature);
    }

    TEST(SemanticAnalyzerTest, RetainsFunctionOverloadsWithDifferentParameterSignatures) {
        constexpr std::string_view source = R"(
func convert() {}
func convert(value: int) {}
func convert(value: string) {}
func convert(first: int, second: string) {}
func convert(first: string, second: int) {}
)";
        const auto config = makeConfig("function_overload_signatures.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_FALSE(result.hasErrors());
        const auto* moduleScope = result.getScopeByAstNode(*module);
        ASSERT_NE(moduleScope, nullptr);
        const auto* functionSymbol = dynamic_cast<const FunctionSymbol*>(moduleScope->lookupLocal("convert"));
        ASSERT_NE(functionSymbol, nullptr);
        const std::vector<std::string_view> expectedNames = {
            "vnl-function-convert()", "vnl-function-convert(int)", "vnl-function-convert(string)", "vnl-function-convert(int, string)", "vnl-function-convert(string, int)",
        };
        const auto& declarations = module->getTopIdentifierDeclarations();
        ASSERT_EQ(declarations.size(), expectedNames.size());
        ASSERT_EQ(functionSymbol->getOverloadings().size(), expectedNames.size());
        for (std::size_t index = 0; index < expectedNames.size(); ++index) {
            SCOPED_TRACE(expectedNames[index]);
            const auto* functionDeclaration = dynamic_cast<const FunctionDeclarationNode*>(declarations[index].get());
            ASSERT_NE(functionDeclaration, nullptr);
            EXPECT_EQ(functionDeclaration->getInternalName(), expectedNames[index]);
            const auto* overloading = functionSymbol->getOverloadingByInternalName(expectedNames[index]);
            ASSERT_NE(overloading, nullptr);
            EXPECT_EQ(overloading->getLocalNode(), functionDeclaration);
        }
    }

    TEST(SemanticAnalyzerTest, GeneratesMemberInternalNamesInTheirDeclaringScopes) {
        constexpr std::string_view source = R"(
type T = int
class Box<T> {
    func read(input: T) -> T { return input }
    func read(input: string) -> string { return input }
    init(input: T) {}
    op +(input: T) -> T { return input }
}
func afterClass(input: T) {}
interface Readable<R> {
    func read(input: R) -> R
    func read(input: string) -> string
    op +(input: R) -> R
}
func afterInterface(input: T) {}
)";
        const auto config = makeConfig("member_internal_name_scopes.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_FALSE(result.hasErrors());
        const auto& declarations = module->getTopIdentifierDeclarations();
        ASSERT_EQ(declarations.size(), 5);
        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(declarations[1].get());
        ASSERT_NE(classDeclaration, nullptr);
        ASSERT_EQ(classDeclaration->getMethodDeclarations().size(), 2);
        ASSERT_EQ(classDeclaration->getConstructorDeclarations().size(), 1);
        ASSERT_EQ(classDeclaration->getOperatorDeclarations().size(), 1);
        EXPECT_EQ(classDeclaration->getMethodDeclarations()[0]->getInternalName(), "vnl-function-read(T)");
        EXPECT_EQ(classDeclaration->getMethodDeclarations()[1]->getInternalName(), "vnl-function-read(string)");
        EXPECT_EQ(classDeclaration->getConstructorDeclarations().front()->getInternalName(), "vnl-constructor(T)");
        EXPECT_EQ(classDeclaration->getOperatorDeclarations().front()->getInternalName(), "vnl-operator-addition(T)");

        const auto* interfaceDeclaration = dynamic_cast<const InterfaceDeclarationNode*>(declarations[3].get());
        ASSERT_NE(interfaceDeclaration, nullptr);
        ASSERT_EQ(interfaceDeclaration->getMethodDeclarations().size(), 2);
        ASSERT_EQ(interfaceDeclaration->getOperatorDeclarations().size(), 1);
        EXPECT_EQ(interfaceDeclaration->getMethodDeclarations()[0]->getInternalName(), "vnl-function-read(R)");
        EXPECT_EQ(interfaceDeclaration->getMethodDeclarations()[1]->getInternalName(), "vnl-function-read(string)");
        EXPECT_EQ(interfaceDeclaration->getOperatorDeclarations().front()->getInternalName(), "vnl-operator-addition(R)");

        const auto* moduleScope = result.getScopeByAstNode(*module);
        ASSERT_NE(moduleScope, nullptr);
        EXPECT_EQ(moduleScope->lookupLocal("read"), nullptr);
        for (const auto* declaration : { declarations[2].get(), declarations[4].get() }) {
            const auto* functionDeclaration = dynamic_cast<const FunctionDeclarationNode*>(declaration);
            ASSERT_NE(functionDeclaration, nullptr);
            const std::string expectedName = "vnl-function-" + std::string(functionDeclaration->getName().getIdentifierString()) + "(int)";
            EXPECT_EQ(functionDeclaration->getInternalName(), expectedName);
            const auto* functionSymbol = dynamic_cast<const FunctionSymbol*>(moduleScope->lookupLocal(functionDeclaration->getName().getIdentifierString()));
            ASSERT_NE(functionSymbol, nullptr);
            const auto* overloading = functionSymbol->getOverloadingByInternalName(expectedName);
            ASSERT_NE(overloading, nullptr);
            EXPECT_EQ(overloading->getLocalNode(), functionDeclaration);
        }

        for (const auto* declaration : { declarations[1].get(), declarations[3].get() }) {
            const auto* scope = result.getScopeByAstNode(*declaration);
            ASSERT_NE(scope, nullptr);
            EXPECT_EQ(scope->findParent(), moduleScope);
            const auto* methodSymbol = dynamic_cast<const FunctionSymbol*>(scope->lookupLocal("read"));
            ASSERT_NE(methodSymbol, nullptr);
            EXPECT_EQ(methodSymbol->getOverloadings().size(), 2);
        }
    }

    TEST(SemanticAnalyzerTest, ContinuesGeneratingInternalNamesAfterInvalidParameterTypes) {
        const std::vector<std::string_view> sources = {
            "func broken(first: int, second: Missing) {}",
            "class Box {\n    func broken(first: int, second: Missing) {}\n}",
            "class Box {\n    init(first: int, second: Missing) {}\n}",
            "class Box {\n    op +(first: int, second: Missing) -> int { return 0 }\n}",
            "interface Readable {\n    func broken(first: int, second: Missing)\n}",
            "interface Readable {\n    op +(first: int, second: Missing) -> int\n}",
        };
        for (const auto source : sources) {
            SCOPED_TRACE(source);
            const auto config = makeConfig("invalid_parameter_internal_names.vnl");
            auto module = parseModule(std::string(source) + "\nfunc after(value: int) {}\n", config);

            const ImportedRootPackageMap imports;
            SemanticAnalyzer analyzer(*module, imports);
            const auto result = analyzer.analyze(config);

            ASSERT_TRUE(result.hasErrors());
            for (const auto& error : result.getErrors()) {
                EXPECT_EQ(error.getMessage(), "Use of undeclared type 'Missing'");
            }
            const auto& declarations = module->getTopIdentifierDeclarations();
            ASSERT_EQ(declarations.size(), 2);
            const auto* functionDeclaration = dynamic_cast<const FunctionDeclarationNode*>(declarations.back().get());
            ASSERT_NE(functionDeclaration, nullptr);
            EXPECT_EQ(functionDeclaration->getInternalName(), "vnl-function-after(int)");
            const auto* moduleScope = result.getScopeByAstNode(*module);
            ASSERT_NE(moduleScope, nullptr);
            const auto* functionSymbol = dynamic_cast<const FunctionSymbol*>(moduleScope->lookupLocal("after"));
            ASSERT_NE(functionSymbol, nullptr);
            const auto* overloading = functionSymbol->getOverloadingByInternalName("vnl-function-after(int)");
            ASSERT_NE(overloading, nullptr);
            EXPECT_EQ(overloading->getLocalNode(), functionDeclaration);
        }
    }

    TEST(SemanticAnalyzerTest, RetainsRegularSymbolsAndScopesWhenCollectingFunctionSymbols) {
        constexpr std::string_view source = R"(
func read(input: Later) -> Later { return input }
class Box<T> {
    value: T
    func read(input: T) -> T { return input }
    init(input: T) {}
    op +(input: T) -> T { return input }
}
interface Readable<T> {
    func read(input: T) -> T
    op +(input: T) -> T
}
class Later {}
)";
        const auto config = makeConfig("symbol_collection_scopes.vnl");
        auto module = parseModule(source, config);

        const auto& declarations = module->getTopIdentifierDeclarations();
        ASSERT_EQ(declarations.size(), 4);
        const auto* functionDeclaration = dynamic_cast<const FunctionDeclarationNode*>(declarations[0].get());
        ASSERT_NE(functionDeclaration, nullptr);
        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(declarations[1].get());
        ASSERT_NE(classDeclaration, nullptr);
        ASSERT_EQ(classDeclaration->getMethodDeclarations().size(), 1);
        const auto* interfaceDeclaration = dynamic_cast<const InterfaceDeclarationNode*>(declarations[2].get());
        ASSERT_NE(interfaceDeclaration, nullptr);
        ASSERT_EQ(interfaceDeclaration->getMethodDeclarations().size(), 1);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_FALSE(result.hasErrors());
        const auto* moduleScope = result.getScopeByAstNode(*module);
        ASSERT_NE(moduleScope, nullptr);
        const auto* functionScope = result.getScopeByAstNode(*functionDeclaration);
        ASSERT_NE(functionScope, nullptr);
        EXPECT_EQ(functionScope->findParent(), moduleScope);
        EXPECT_NE(functionScope->lookupLocal("input"), nullptr);
        EXPECT_EQ(functionScope->lookup("Later"), moduleScope->lookupLocal("Later"));

        const auto* classScope = result.getScopeByAstNode(*classDeclaration);
        ASSERT_NE(classScope, nullptr);
        EXPECT_EQ(classScope->findParent(), moduleScope);
        EXPECT_NE(classScope->lookupLocal("value"), nullptr);
        EXPECT_NE(classScope->lookupLocal("T"), nullptr);
        EXPECT_NE(dynamic_cast<const FunctionSymbol*>(classScope->lookupLocal("read")), nullptr);
        ASSERT_EQ(classDeclaration->getConstructorDeclarations().size(), 1);
        ASSERT_EQ(classDeclaration->getOperatorDeclarations().size(), 1);
        const std::vector<const AstNode*> classCallables = {
            classDeclaration->getMethodDeclarations().front().get(),
            classDeclaration->getConstructorDeclarations().front().get(),
            classDeclaration->getOperatorDeclarations().front().get(),
        };
        for (const auto* callable : classCallables) {
            const auto* scope = result.getScopeByAstNode(*callable);
            ASSERT_NE(scope, nullptr);
            EXPECT_EQ(scope->findParent(), classScope);
            EXPECT_NE(scope->lookupLocal("input"), nullptr);
            EXPECT_EQ(scope->lookup("T"), classScope->lookupLocal("T"));
        }

        const auto* interfaceScope = result.getScopeByAstNode(*interfaceDeclaration);
        ASSERT_NE(interfaceScope, nullptr);
        EXPECT_EQ(interfaceScope->findParent(), moduleScope);
        EXPECT_NE(interfaceScope->lookupLocal("T"), nullptr);
        EXPECT_NE(dynamic_cast<const FunctionSymbol*>(interfaceScope->lookupLocal("read")), nullptr);
        ASSERT_EQ(interfaceDeclaration->getOperatorDeclarations().size(), 1);
        const std::vector<const AstNode*> interfaceCallables = {
            interfaceDeclaration->getMethodDeclarations().front().get(),
            interfaceDeclaration->getOperatorDeclarations().front().get(),
        };
        for (const auto* callable : interfaceCallables) {
            const auto* scope = result.getScopeByAstNode(*callable);
            ASSERT_NE(scope, nullptr);
            EXPECT_EQ(scope->findParent(), interfaceScope);
            EXPECT_NE(scope->lookupLocal("input"), nullptr);
            EXPECT_EQ(scope->lookup("T"), interfaceScope->lookupLocal("T"));
        }
    }

    TEST(SemanticAnalyzerTest, RejectsFunctionAndNonFunctionNameCollisions) {
        constexpr std::string_view source = R"(
let value = 0
func value(input: int) {}
)";
        const auto config = makeConfig("function_collision.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_TRUE(result.hasErrors());
        ASSERT_EQ(result.getErrors().size(), 1);
        EXPECT_EQ(result.getErrors()[0].getMessage(), "Redeclaration of symbol 'value'");
    }

    TEST(SemanticAnalyzerTest, RejectsDuplicateFunctionSignaturesAfterResolvingAliases) {
        struct TestCase {
            std::string_view source;
            std::string_view expectedMessage;
        };
        constexpr TestCase testCases[] = {
            {
                R"(
type Count = int
func read(first: Count) -> int { return 0 }
func read(second: int) -> string { return "value" }
)",
                "Redeclaration of symbol 'vnl-function-read(int)'",
            },
            {
                R"(
type Count = int
class Reader {
    func read(first: Count) -> int { return 0 }
    func read(second: int) -> string { return "value" }
}
)",
                "Redeclaration of class member 'vnl-function-read(int)'",
            },
            {
                R"(
type Count = int
interface Reader {
    func read(first: Count) -> int
    func read(second: int) -> string
}
)",
                "Redeclaration of interface method 'vnl-function-read(int)'",
            },
        };

        for (const auto& testCase : testCases) {
            SCOPED_TRACE(testCase.source);
            const auto config = makeConfig("duplicate_function_signatures.vnl");
            auto module = parseModule(testCase.source, config);

            const ImportedRootPackageMap imports;
            SemanticAnalyzer analyzer(*module, imports);
            const auto result = analyzer.analyze(config);

            ASSERT_TRUE(result.hasErrors());
            ASSERT_EQ(result.getErrors().size(), 1);
            EXPECT_EQ(result.getErrors().front().getMessage(), testCase.expectedMessage);
        }
    }

    TEST(SemanticAnalyzerTest, RejectsDuplicateConstructorSignatures) {
        struct TestCase {
            std::string_view source;
            std::string_view expectedMessage;
        };
        constexpr TestCase testCases[] = {
            {
                R"(
class Value {
    init() {}
    init() {}
}
)",
                "Redeclaration of class member 'vnl-constructor()'",
            },
            {
                R"(
type Count = int
class Value {
    init(first: Count) {}
    init(second: int) {}
}
)",
                "Redeclaration of class member 'vnl-constructor(int)'",
            },
        };

        for (const auto& testCase : testCases) {
            SCOPED_TRACE(testCase.source);
            const auto config = makeConfig("duplicate_constructor_signatures.vnl");
            auto module = parseModule(testCase.source, config);

            const ImportedRootPackageMap imports;
            SemanticAnalyzer analyzer(*module, imports);
            const auto result = analyzer.analyze(config);

            ASSERT_TRUE(result.hasErrors());
            ASSERT_EQ(result.getErrors().size(), 1);
            EXPECT_EQ(result.getErrors().front().getMessage(), testCase.expectedMessage);
        }
    }

    TEST(SemanticAnalyzerTest, RejectsDuplicateOperatorSignatures) {
        struct TestCase {
            std::string_view source;
            std::string_view expectedMessage;
        };
        constexpr TestCase testCases[] = {
            {
                R"(
class Callable {
    op ()() -> int { return 0 }
    op ()() -> string { return "value" }
}
)",
                "Redeclaration of class member 'vnl-operator-call()'",
            },
            {
                R"(
interface Callable {
    op ()() -> int
    op ()() -> string
}
)",
                "Redeclaration of interface operator 'vnl-operator-call()'",
            },
            {
                R"(
type Count = int
class Addable {
    op +(first: Count) -> int { return 0 }
    op +(second: int) -> string { return "value" }
}
)",
                "Redeclaration of class member 'vnl-operator-addition(int)'",
            },
            {
                R"(
type Count = int
interface Addable {
    op +(first: Count) -> int
    op +(second: int) -> string
}
)",
                "Redeclaration of interface operator 'vnl-operator-addition(int)'",
            },
        };

        for (const auto& testCase : testCases) {
            SCOPED_TRACE(testCase.source);
            const auto config = makeConfig("duplicate_operator_signatures.vnl");
            auto module = parseModule(testCase.source, config);

            const ImportedRootPackageMap imports;
            SemanticAnalyzer analyzer(*module, imports);
            const auto result = analyzer.analyze(config);

            ASSERT_TRUE(result.hasErrors());
            ASSERT_EQ(result.getErrors().size(), 1);
            EXPECT_EQ(result.getErrors().front().getMessage(), testCase.expectedMessage);
        }
    }

    TEST(SemanticAnalyzerTest, AcceptsDistinctConstructorAndOperatorSignatures) {
        constexpr std::string_view source = R"(
class Callable {
    init() {}
    init(value: int) {}
    init(value: string) {}
    op ()() -> int { return 0 }
    op ()(value: int) -> int { return value }
    op ()(value: string) -> string { return value }
}
interface Invocable {
    op ()() -> int
    op ()(value: int) -> int
    op ()(value: string) -> string
}
)";
        const auto config = makeConfig("distinct_constructor_operator_signatures.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_FALSE(result.hasErrors());
        const auto& declarations = module->getTopIdentifierDeclarations();
        ASSERT_EQ(declarations.size(), 2);
        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(declarations[0].get());
        ASSERT_NE(classDeclaration, nullptr);
        const auto* interfaceDeclaration = dynamic_cast<const InterfaceDeclarationNode*>(declarations[1].get());
        ASSERT_NE(interfaceDeclaration, nullptr);
        ASSERT_EQ(classDeclaration->getConstructorDeclarations().size(), 3);
        ASSERT_EQ(classDeclaration->getOperatorDeclarations().size(), 3);
        ASSERT_EQ(interfaceDeclaration->getOperatorDeclarations().size(), 3);

        constexpr std::string_view constructorNames[] = {
            "vnl-constructor()",
            "vnl-constructor(int)",
            "vnl-constructor(string)",
        };
        constexpr std::string_view operatorNames[] = {
            "vnl-operator-call()",
            "vnl-operator-call(int)",
            "vnl-operator-call(string)",
        };
        for (std::size_t index = 0; index < 3; ++index) {
            SCOPED_TRACE(index);
            EXPECT_EQ(classDeclaration->getConstructorDeclarations()[index]->getInternalName(), constructorNames[index]);
            EXPECT_EQ(classDeclaration->getOperatorDeclarations()[index]->getInternalName(), operatorNames[index]);
            EXPECT_EQ(interfaceDeclaration->getOperatorDeclarations()[index]->getInternalName(), operatorNames[index]);
        }
    }

    TEST(SemanticAnalyzerTest, AcceptsNativeOperatorsWithoutBodies) {
        constexpr std::string_view source = R"(
class Vec3 {
    native op +(other: Vec3) -> Vec3
    override native op ==(other: Vec3) -> bool
    op -(other: Vec3) -> Vec3 { return this }
}
)";
        const auto config = makeConfig("native_operators.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_FALSE(result.hasErrors());
        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(module->getTopIdentifierDeclarations().front().get());
        ASSERT_NE(classDeclaration, nullptr);
        ASSERT_EQ(classDeclaration->getOperatorDeclarations().size(), 3);

        const std::string vec3TypeName = std::string(module->getFullName()) + ".Vec3";
        EXPECT_EQ(classDeclaration->getOperatorDeclarations()[0]->getInternalName(), "vnl-operator-addition(" + vec3TypeName + ")");
        EXPECT_EQ(classDeclaration->getOperatorDeclarations()[1]->getInternalName(), "vnl-operator-equality(" + vec3TypeName + ")");
        EXPECT_EQ(classDeclaration->getOperatorDeclarations()[2]->getInternalName(), "vnl-operator-subtraction(" + vec3TypeName + ")");
    }

    TEST(SemanticAnalyzerTest, RejectsEqualityOperatorsNotReturningBool) {
        constexpr std::string_view source = R"(
class Vec3 {
    op ==(other: Vec3) -> int { return 0 }
    op ==(other: int) -> string { return "" }
    native op ==(other: string)
}
interface Equatable {
    op ==(other: Equatable) -> Equatable
}
)";
        const auto config = makeConfig("equality_operator_errors.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_EQ(result.getErrors().size(), 4);
        for (const auto& error : result.getErrors()) {
            EXPECT_EQ(error.getMessage(), "Operator 'equality' must return 'bool'");
        }
    }

    TEST(SemanticAnalyzerTest, RejectsDuplicateSignaturesBetweenNativeAndRegularOperators) {
        constexpr std::string_view source = R"(
class Addable {
    op +(first: int) -> int { return first }
    native op +(second: int) -> int
}
)";
        const auto config = makeConfig("duplicate_native_operator_signatures.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_TRUE(result.hasErrors());
        ASSERT_EQ(result.getErrors().size(), 1);
        EXPECT_EQ(result.getErrors().front().getMessage(), "Redeclaration of class member 'vnl-operator-addition(int)'");
    }

    TEST(SemanticAnalyzerTest, AcceptsInheritanceFromNonFinalClassesAndImplementationsOfInterfaces) {
        constexpr std::string_view source = R"(
class Base {}
class Box<T> {}
interface Readable {}
interface Writable<T> {}
type BaseAlias = Base
type ReadableAlias = Readable
class Derived extends Base implements Readable, Writable<int> {}
class Boxed extends Box<int> {}
class Aliased extends BaseAlias implements ReadableAlias {}
)";
        const auto config = makeConfig("valid_inheritance.vnl");
        auto module = parseModule(source, config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_FALSE(result.hasErrors()) << result.getErrors().front().getMessage();
    }

    TEST(SemanticAnalyzerTest, RejectsInheritanceFromPrimitiveTypes) {
        const auto config = makeConfig("primitive_base_class.vnl");
        auto module = parseModule("class Derived extends int {}", config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_EQ(result.getErrors().size(), 1);
        EXPECT_EQ(result.getErrors().front().getMessage(), "Inheritance from primitive types is not allowed");
    }

    TEST(SemanticAnalyzerTest, RejectsInheritanceFromNonClassTypes) {
        const std::vector<std::string_view> sources = {
            "interface Readable {}\nclass Derived extends Readable {}",
            "enum State {\n    Ready\n}\nclass Derived extends State {}",
            "enum State {\n    Ready\n}\nclass Derived extends State.Ready {}",
            "class Derived<T> extends T {}",
            "final class Derived<T> extends T {}",
        };
        for (const auto source : sources) {
            SCOPED_TRACE(source);
            const auto config = makeConfig("non_class_base_class.vnl");
            auto module = parseModule(source, config);

            const ImportedRootPackageMap imports;
            SemanticAnalyzer analyzer(*module, imports);
            const auto result = analyzer.analyze(config);

            ASSERT_EQ(result.getErrors().size(), 1);
            EXPECT_EQ(result.getErrors().front().getMessage(), "Inheritance from non-class types is not allowed");
            const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(module->getTopIdentifierDeclarations().back().get());
            ASSERT_NE(classDeclaration, nullptr);
            const auto* inheritanceNode = result.getInheritanceNodeByLocalNode(classDeclaration);
            ASSERT_NE(inheritanceNode, nullptr);
            EXPECT_EQ(inheritanceNode->findParent(), nullptr);
        }
    }

    TEST(SemanticAnalyzerTest, RejectsInheritanceFromFinalClasses) {
        const std::vector<std::string_view> sources = {
            "final class Base {}\nclass Derived extends Base {}",
            "final class Box<T> {}\nclass Derived extends Box<int> {}",
            "final class Base {}\ntype Alias = Base\nclass Derived extends Alias {}",
        };
        for (const auto source : sources) {
            SCOPED_TRACE(source);
            const auto config = makeConfig("final_base_class.vnl");
            auto module = parseModule(source, config);

            const ImportedRootPackageMap imports;
            SemanticAnalyzer analyzer(*module, imports);
            const auto result = analyzer.analyze(config);

            ASSERT_EQ(result.getErrors().size(), 1);
            EXPECT_EQ(result.getErrors().front().getMessage(), "Inheritance from final classes is not allowed");
        }
    }

    TEST(SemanticAnalyzerTest, RejectsCyclicInheritanceForEveryClassInTheCycle) {
        const std::vector<std::pair<std::string_view, std::size_t>> cases = {
            { "class Self extends Self {}", 1 },
            { "class First extends Second {}\nclass Second extends First {}", 2 },
            { "class First extends Second {}\nclass Second extends Third {}\nclass Third extends First {}", 3 },
        };
        for (const auto& [source, errorCount] : cases) {
            SCOPED_TRACE(source);
            const auto config = makeConfig("cyclic_inheritance.vnl");
            auto module = parseModule(source, config);

            const ImportedRootPackageMap imports;
            SemanticAnalyzer analyzer(*module, imports);
            const auto result = analyzer.analyze(config);

            ASSERT_EQ(result.getErrors().size(), errorCount);
            for (const auto& error : result.getErrors()) {
                EXPECT_EQ(error.getMessage(), "Cyclic inheritance detected");
            }
        }
    }

    TEST(SemanticAnalyzerTest, RejectsImplementationsOfNonInterfaceTypes) {
        struct InvalidImplementation {
            std::string_view source;
            std::string_view typeName;
            bool isLocalType;
        };

        const std::vector<InvalidImplementation> cases = {
            { "class Derived implements int {}", "int", false },
            { "class Base {}\nclass Derived implements Base {}", "Base", true },
            { "enum State {\n    Ready\n}\nclass Derived implements State {}", "State", true },
            { "class Derived<T> implements T {}", "T", false },
        };
        for (const auto& [source, typeName, isLocalType] : cases) {
            SCOPED_TRACE(source);
            const auto config = makeConfig("non_interface_implementations.vnl");
            auto module = parseModule(source, config);

            const ImportedRootPackageMap imports;
            SemanticAnalyzer analyzer(*module, imports);
            const auto result = analyzer.analyze(config);

            const std::string fullTypeName = isLocalType ? std::string(module->getFullName()) + "." + std::string(typeName) : std::string(typeName);
            ASSERT_EQ(result.getErrors().size(), 1);
            EXPECT_EQ(result.getErrors().front().getMessage(), "Type '" + fullTypeName + "' is not an interface");
        }
    }

    TEST(SemanticAnalyzerTest, ReportsEveryNonInterfaceTypeInImplementationLists) {
        const auto config = makeConfig("mixed_implementations.vnl");
        auto module = parseModule("interface Readable {}\nclass Derived implements Readable, int, string {}", config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_EQ(result.getErrors().size(), 2);
        EXPECT_EQ(result.getErrors()[0].getMessage(), "Type 'int' is not an interface");
        EXPECT_EQ(result.getErrors()[1].getMessage(), "Type 'string' is not an interface");
    }

    TEST(SemanticAnalyzerTest, ReportsUndeclaredBaseClassesAndInterfacesOnce) {
        const auto config = makeConfig("undeclared_inheritance.vnl");
        auto module = parseModule("class Derived extends Missing implements AlsoMissing {}", config);

        const ImportedRootPackageMap imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_EQ(result.getErrors().size(), 2);
        EXPECT_EQ(result.getErrors()[0].getMessage(), "Use of undeclared type 'Missing'");
        EXPECT_EQ(result.getErrors()[1].getMessage(), "Use of undeclared type 'AlsoMissing'");
    }

    TEST(SemanticContextTest, RetainsPoppedScopesAndTheirParents) {
        const auto config = makeConfig("scopes.vnl");
        const auto module = parseModule("class Sample {}", config);
        const auto* classDeclaration = module->getTopIdentifierDeclarations().front().get();
        const ImportedRootPackageMap imports;
        SemanticContext context(*module, imports);

        context.pushScope(std::make_unique<Scope>(ScopeKind::MODULE, nullptr, module.get()));
        const auto* moduleScope = &context.currentScope();
        ASSERT_TRUE(context.currentScope().declare(RegularSymbol(RegularSymbolKind::CLASS, RegularSymbolAccessModifier::PUBLIC, "Sample", classDeclaration)));
        context.pushScope(std::make_unique<Scope>(ScopeKind::CLASS, moduleScope, classDeclaration));
        const auto* classScope = &context.currentScope();

        EXPECT_EQ(context.getScopeByAstNode(module.get()), moduleScope);
        EXPECT_EQ(context.getScopeByAstNode(classDeclaration), classScope);
        EXPECT_EQ(context.getScopeByAstNode(nullptr), nullptr);

        context.popScope();
        EXPECT_EQ(&context.currentScope(), moduleScope);
        EXPECT_EQ(context.getScopeByAstNode(classDeclaration), classScope);
        context.popScope();

        ASSERT_EQ(context.getScopeByAstNode(module.get()), moduleScope);
        ASSERT_EQ(context.getScopeByAstNode(classDeclaration), classScope);
        EXPECT_EQ(classScope->findParent(), moduleScope);
        const auto* symbol = asRegularSymbol(classScope->lookup("Sample"));
        ASSERT_NE(symbol, nullptr);
        EXPECT_EQ(symbol->getLocalNode(), classDeclaration);
    }

    TEST(SemanticContextTest, RetainsLocalAndImportedScopesSeparately) {
        const auto config = makeConfig("scopes.vnl");
        const auto module = parseModule("", config);
        const ImportedModule importedModule("api", {});
        const ImportedModule anotherModule("other", {});
        const ImportedRootPackageMap imports;
        SemanticContext context(*module, imports);

        context.pushScope(std::make_unique<Scope>(ScopeKind::MODULE, nullptr, module.get()));
        const auto* localScope = &context.currentScope();
        EXPECT_EQ(localScope->getOrigin(), ScopeOrigin::LOCAL);
        EXPECT_EQ(localScope->getLocalNode(), module.get());
        EXPECT_EQ(localScope->getImportedNode(), nullptr);

        context.pushScope(std::make_unique<Scope>(ScopeKind::MODULE, nullptr, &importedModule));
        const auto* importedScope = &context.currentScope();
        EXPECT_EQ(importedScope->getOrigin(), ScopeOrigin::IMPORTED);
        EXPECT_EQ(importedScope->getLocalNode(), nullptr);
        EXPECT_EQ(importedScope->getImportedNode(), &importedModule);
        EXPECT_EQ(context.getScopeByAstNode(nullptr), nullptr);
        EXPECT_EQ(context.getScopeByImportedNode(nullptr), nullptr);
        EXPECT_EQ(context.getScopeByImportedNode(&importedModule), importedScope);
        EXPECT_EQ(context.getScopeByAstNode(module.get()), localScope);
        EXPECT_EQ(&context.getOrCreateImportedScope(ScopeKind::MODULE, nullptr, importedModule), importedScope);

        context.popScope();
        EXPECT_EQ(context.getScopeByImportedNode(&importedModule), importedScope);
        EXPECT_EQ(&context.getOrCreateImportedScope(ScopeKind::MODULE, nullptr, importedModule), importedScope);
        context.pushScope(std::make_unique<Scope>(ScopeKind::MODULE, nullptr, &anotherModule));
        const auto* anotherScope = &context.currentScope();
        context.popScope();
        context.popScope();

        EXPECT_EQ(context.getScopeByAstNode(module.get()), localScope);
        EXPECT_EQ(context.getScopeByImportedNode(&importedModule), importedScope);
        EXPECT_EQ(context.getScopeByImportedNode(&anotherModule), anotherScope);
        auto importedScopes = context.takeImportedScopeMap();
        ASSERT_EQ(importedScopes.size(), 2);
        EXPECT_EQ(importedScopes.at(&importedModule).get(), importedScope);
        EXPECT_EQ(importedScopes.at(&anotherModule).get(), anotherScope);
        auto localScopes = context.takeLocalScopeMap();
        ASSERT_EQ(localScopes.size(), 1);
        EXPECT_EQ(localScopes.at(module.get()).get(), localScope);
    }

    class SemanticAnalyzerImportTest : public testing::Test {
    protected:
        std::filesystem::path testDirectory;
        Config config = makeConfig("imports.vnl");
        std::shared_ptr<const ModuleNode> module;
        std::optional<CollectionResult> collectionResult;

        void SetUp() override {
            const auto* testInfo = testing::UnitTest::GetInstance()->current_test_info();
            testDirectory = std::filesystem::temp_directory_path() / ("vnlctest-import-" + std::string(testInfo->name()));
            std::filesystem::remove_all(testDirectory);
            config.dependencyPackageRootPaths.emplace("pkg", testDirectory / "dependency_source");
            config.dependencyPackageRootPaths.emplace("extra", testDirectory / "another_source");
            config.dependencyPackageRootPaths.emplace("vanillang", testDirectory / "standard_library");
            writeFile("standard_library/typesystem.vni", R"({
    "Optional": {"category": "enum", "genericParameters": ["T"], "members": {}},
    "Ordering": {"category": "enum", "genericParameters": [],
        "members": {
            "Less": {"category": "enummember", "associatedValues": {}},
            "Equal": {"category": "enummember", "associatedValues": {}},
            "Greater": {"category": "enummember", "associatedValues": {}}}}
})");
            writeFile("dependency_source/api.vni", R"({"value":{"category":"let","type":"int"},"count":{"category":"let","type":"int"}})");
            writeFile("dependency_source/sub/other.vni", R"({"flag":{"category":"let","type":"bool"}})");
            writeFile("dependency_source/sub/second.vni", R"({"caption":{"category":"let","type":"string"}})");
            writeFile("another_source/tools.vni", R"({"enabled":{"category":"let","type":"bool"}})");
        }

        void TearDown() override {
            std::error_code error;
            std::filesystem::remove_all(testDirectory, error);
        }

        void writeFile(const std::filesystem::path& relativePath, std::string_view contents) const {
            const auto path = testDirectory / relativePath;
            std::filesystem::create_directories(path.parent_path());
            std::ofstream output(path);
            output << contents;
        }

        void writeScopedModule() const {
            writeFile("dependency_source/api.vni", R"({
    "value": {"category": "let", "type": "int"},
    "run": {"category": "func", "name": "run", "returnType": "void", "native": false,
        "parameters": {"amount": {"category": "parameter", "type": "int"}}},
    "Box": {"category": "class", "genericParameters": ["T"], "baseClass": null, "implementedInterfaces": [], "final": false,
        "properties": {
            "visible": {"category": "property", "type": "T", "static": false, "accessModifier": "public"},
            "secret": {"category": "property", "type": "int", "static": true, "accessModifier": "private"}},
        "methods": {"apply": {"category": "method", "name": "apply", "returnType": "T", "native": false, "static": false, "accessModifier": "protected",
            "parameters": {"input": {"category": "parameter", "type": "T"}}}},
        "constructors": {}, "operators": {}},
    "Readable": {"category": "interface", "genericParameters": ["R"],
        "methods": {"read": {"category": "method", "name": "read", "returnType": "R", "native": false, "static": false, "accessModifier": "public",
            "parameters": {"input": {"category": "parameter", "type": "R"}}}}, "operators": {}},
    "State": {"category": "enum", "genericParameters": ["E"],
        "members": {"Ready": {"category": "enummember", "associatedValues": {"payload": {"category": "enumvalue", "type": "E"}}}}},
    "Alias": {"category": "typealias", "genericParameters": ["A"], "originalType": "A"},
    "External": {"category": "imported", "source": "extra.tools.enabled"},
    "method": {"category": "method", "name": "method", "returnType": "void", "native": false, "static": true, "accessModifier": "public",
        "parameters": {"amount": {"category": "parameter", "type": "int"}}},
    "Ready": {"category": "enummember", "associatedValues": {"payload": {"category": "enumvalue", "type": "int"}}},
    "property": {"category": "property", "type": "int", "static": true, "accessModifier": "public"},
    "parameter": {"category": "parameter", "type": "int"}
})");
        }

        void writeInheritanceModule() const {
            writeFile("dependency_source/api.vni", R"({
    "Base": {"category": "class", "genericParameters": [], "properties": {}, "methods": {}, "constructors": {}, "operators": {},
        "baseClass": null, "implementedInterfaces": [], "final": false},
    "Sealed": {"category": "class", "genericParameters": [], "properties": {}, "methods": {}, "constructors": {}, "operators": {},
        "baseClass": null, "implementedInterfaces": [], "final": true},
    "Readable": {"category": "interface", "genericParameters": [], "methods": {}, "operators": {}},
    "State": {"category": "enum", "genericParameters": [], "members": {"Ready": {"category": "enummember", "associatedValues": {}}}}
})");
        }

        void prepareImports(std::string_view source) {
            collectionResult = collectModule(source, config);
            EXPECT_TRUE(collectionResult->getErrors().empty());
        }

        const ImportedPackage* getImportedRootPackageByName(std::string_view name) const {
            const auto& imports = collectionResult->getImports();
            const auto it = imports.find(std::string(name));
            return it == imports.end() ? nullptr : it->second.get();
        }

        SemanticResult analyze(std::string_view source, std::optional<std::string_view> importSource = std::nullopt) {
            prepareImports(importSource.value_or(source));
            const auto& imports = collectionResult->getImports();
            module = parseModule(source, config);
            SemanticAnalyzer analyzer(*module, imports);
            return analyzer.analyze(config);
        }

        template <typename Callback> void withAliasType(std::string_view source, Callback&& callback) {
            prepareImports(source);
            module = parseModule(source, config);
            const auto& imports = collectionResult->getImports();
            SemanticAnalyzer analyzer(*module, imports);
            analyzer.checkModule(*module, config);
            const TypeAliasDeclarationNode* typeAlias = nullptr;
            for (const auto& topIdentifierDecl : module->getTopIdentifierDeclarations()) {
                if (const auto* typeAliasDecl = dynamic_cast<const TypeAliasDeclarationNode*>(topIdentifierDecl.get())) {
                    typeAlias = typeAliasDecl;
                }
            }
            if (typeAlias == nullptr) {
                return;
            }

            auto& context = analyzer.context;
            if (!context.pushExistingScope(typeAlias)) {
                return;
            }
            callback(analyzer, *typeAlias);
            context.popScope();
        }

        SemanticContext& semanticContext(SemanticAnalyzer& analyzer) const {
            return analyzer.context;
        }

        const Type* checkedType(SemanticAnalyzer& analyzer, const TypeReferenceNode& type) const {
            analyzer.resolveTypeReference(type);
            return analyzer.context.getTypeByTypeReferenceNode(&type);
        }

        const ImportedItem* findImportedNode(const SemanticResult& result, std::string_view name) const {
            const auto* scope = result.getScopeByAstNode(*module);
            EXPECT_NE(scope, nullptr);
            if (scope == nullptr) return nullptr;
            const Symbol* symbol = scope->lookupLocal(name);
            if (const auto* regularSymbol = dynamic_cast<const RegularSymbol*>(symbol)) {
                return regularSymbol->getImportedNode();
            }
            if (const auto* functionSymbol = dynamic_cast<const FunctionSymbol*>(symbol); functionSymbol != nullptr && functionSymbol->isUnique()) {
                return functionSymbol->getOverloadings().begin()->second.getImportedNode();
            }
            return nullptr;
        }
    };

    TEST_F(SemanticAnalyzerImportTest, MapsPrimitiveTypeReferenceNodesToSharedTypes) {
        withAliasType("type Result = int\n", [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
            auto& context = semanticContext(analyzer);
            const auto* type = context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType());
            ASSERT_NE(type, nullptr);
            EXPECT_EQ(type, PrimitiveType::intType());
            EXPECT_EQ(checkedType(analyzer, typeAlias.getOriginalType()), type);
        });
    }

    TEST_F(SemanticAnalyzerImportTest, RegistersOptionalPrimitiveTypesWithTheirOriginalArguments) {
        for (const auto* primitiveType : { PrimitiveType::byteType(),
                                           PrimitiveType::shortType(),
                                           PrimitiveType::intType(),
                                           PrimitiveType::longType(),
                                           PrimitiveType::floatType(),
                                           PrimitiveType::doubleType(),
                                           PrimitiveType::booleanType(),
                                           PrimitiveType::stringType() }) {
            const auto primitiveName = std::string(primitiveType->getFullTypeName());
            SCOPED_TRACE(primitiveName);
            withAliasType(
                "import vanillang.typesystem.Optional\ntype Result = " + primitiveName + "?\n",
                [this, primitiveType, &primitiveName](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
                    auto& context = semanticContext(analyzer);
                    ASSERT_TRUE(context.getErrors().empty()) << context.getErrors().front().getMessage();
                    const auto* optionalType = dynamic_cast<const CustomizedType*>(context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType()));
                    ASSERT_NE(optionalType, nullptr);
                    EXPECT_EQ(optionalType->getCustomizedKind(), CustomizedTypeKind::ENUM);
                    EXPECT_EQ(optionalType->getFullTypeName(), "vanillang.typesystem.Optional<" + primitiveName + ">");
                    ASSERT_EQ(optionalType->getGenericArguments().size(), 1);
                    EXPECT_EQ(optionalType->getGenericArguments().front(), primitiveType);
                    EXPECT_EQ(context.getCustomizedTypeByFullTypeName(std::string(optionalType->getFullTypeName())), optionalType);
                    EXPECT_EQ(checkedType(analyzer, typeAlias.getOriginalType()), optionalType);
                }
            );
        }
    }

    TEST_F(SemanticAnalyzerImportTest, RegistersNestedOptionalLocalTypesWithTheirOriginalArguments) {
        withAliasType("import vanillang.typesystem.Optional\nclass Box<T> {}\ntype Result = Box<int?>?\n", [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
            auto& context = semanticContext(analyzer);
            ASSERT_TRUE(context.getErrors().empty());
            const auto* optionalType = dynamic_cast<const CustomizedType*>(context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType()));
            ASSERT_NE(optionalType, nullptr);
            EXPECT_EQ(optionalType->getCustomizedKind(), CustomizedTypeKind::ENUM);
            const auto boxName = std::string(module->getFullName()) + ".Box<vanillang.typesystem.Optional<int>>";
            EXPECT_EQ(optionalType->getFullTypeName(), "vanillang.typesystem.Optional<" + boxName + ">");
            ASSERT_EQ(optionalType->getGenericArguments().size(), 1);
            const auto* boxType = dynamic_cast<const CustomizedType*>(optionalType->getGenericArguments().front());
            ASSERT_NE(boxType, nullptr);
            EXPECT_EQ(boxType->getFullTypeName(), boxName);
            EXPECT_EQ(boxType->getCustomizedKind(), CustomizedTypeKind::CLASS);
            EXPECT_EQ(boxType->getOrigin(), CustomizedTypeOrigin::LOCAL);
            EXPECT_EQ(boxType->getLocalNode(), module->getTopIdentifierDeclarations().front().get());
            EXPECT_EQ(context.getCustomizedTypeByFullTypeName(boxName), boxType);
            ASSERT_EQ(boxType->getGenericArguments().size(), 1);
            const auto* optionalIntType = dynamic_cast<const CustomizedType*>(boxType->getGenericArguments().front());
            ASSERT_NE(optionalIntType, nullptr);
            EXPECT_EQ(optionalIntType->getCustomizedKind(), CustomizedTypeKind::ENUM);
            EXPECT_EQ(optionalIntType->getFullTypeName(), "vanillang.typesystem.Optional<int>");
            const auto& originalType = dynamic_cast<const CustomizedTypeReferenceNode&>(typeAlias.getOriginalType());
            EXPECT_EQ(context.getTypeByTypeReferenceNode(originalType.getGenericArguments().front().get()), optionalIntType);
            ASSERT_EQ(optionalIntType->getGenericArguments().size(), 1);
            EXPECT_EQ(optionalIntType->getGenericArguments().front(), PrimitiveType::intType());
            EXPECT_EQ(context.getCustomizedTypeByFullTypeName(std::string(optionalType->getFullTypeName())), optionalType);
            EXPECT_EQ(checkedType(analyzer, typeAlias.getOriginalType()), optionalType);
        });
    }

    TEST_F(SemanticAnalyzerImportTest, RegistersOptionalImportedTypesWithTheirOriginalDeclarations) {
        writeScopedModule();
        withAliasType(
            "import pkg.api.Box as ImportedBox\nimport vanillang.typesystem.Optional\ntype Result = ImportedBox<int>?\n",
            [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
                auto& context = semanticContext(analyzer);
                ASSERT_TRUE(context.getErrors().empty());
                const auto* optionalType = dynamic_cast<const CustomizedType*>(context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType()));
                ASSERT_NE(optionalType, nullptr);
                EXPECT_EQ(optionalType->getCustomizedKind(), CustomizedTypeKind::ENUM);
                EXPECT_EQ(optionalType->getFullTypeName(), "vanillang.typesystem.Optional<pkg.api.Box<int>>");
                ASSERT_EQ(optionalType->getGenericArguments().size(), 1);
                const auto* boxType = dynamic_cast<const CustomizedType*>(optionalType->getGenericArguments().front());
                ASSERT_NE(boxType, nullptr);
                EXPECT_EQ(boxType->getFullTypeName(), "pkg.api.Box<int>");
                EXPECT_EQ(boxType->getCustomizedKind(), CustomizedTypeKind::CLASS);
                EXPECT_EQ(boxType->getOrigin(), CustomizedTypeOrigin::IMPORTED);
                const auto* symbol = asRegularSymbol(context.currentScope().lookup("ImportedBox"));
                ASSERT_NE(symbol, nullptr);
                EXPECT_EQ(boxType->getImportedNode(), symbol->getImportedNode());
                ASSERT_EQ(boxType->getGenericArguments().size(), 1);
                EXPECT_EQ(boxType->getGenericArguments().front(), PrimitiveType::intType());
                EXPECT_EQ(context.getCustomizedTypeByFullTypeName("pkg.api.Box<int>"), boxType);
                EXPECT_EQ(checkedType(analyzer, typeAlias.getOriginalType()), optionalType);
            }
        );
    }

    TEST_F(SemanticAnalyzerImportTest, RegistersOptionalGenericParametersWithTheirOriginalDeclarations) {
        withAliasType("import vanillang.typesystem.Optional\ntype Result<T> = T?\n", [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
            auto& context = semanticContext(analyzer);
            ASSERT_TRUE(context.getErrors().empty());
            const auto* optionalType = dynamic_cast<const CustomizedType*>(context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType()));
            ASSERT_NE(optionalType, nullptr);
            EXPECT_EQ(optionalType->getCustomizedKind(), CustomizedTypeKind::ENUM);
            EXPECT_EQ(optionalType->getFullTypeName(), "vanillang.typesystem.Optional<T>");
            ASSERT_EQ(optionalType->getGenericArguments().size(), 1);
            const auto* genericType = dynamic_cast<const CustomizedType*>(optionalType->getGenericArguments().front());
            ASSERT_NE(genericType, nullptr);
            EXPECT_EQ(genericType->getCustomizedKind(), CustomizedTypeKind::GENERIC_PARAMETER);
            EXPECT_EQ(genericType->getFullTypeName(), "T");
            EXPECT_EQ(genericType->getLocalNode(), &typeAlias);
            EXPECT_EQ(context.getCustomizedTypeByFullTypeName("T"), genericType);
        });
    }

    TEST_F(SemanticAnalyzerImportTest, RetainsSharedOptionalTypesAndTheirOriginalTypesInAnalysisResults) {
        const auto result = analyze("import vanillang.typesystem.Optional\nclass Box<T> {}\ntype Original = Box<int>\ntype First = Box<int>?\ntype Second = Box<int>?\n");
        ASSERT_FALSE(result.hasErrors());
        const auto& declarations = module->getTopIdentifierDeclarations();
        const auto& originalAlias = dynamic_cast<const TypeAliasDeclarationNode&>(*declarations.at(1));
        const auto& firstAlias = dynamic_cast<const TypeAliasDeclarationNode&>(*declarations.at(2));
        const auto& secondAlias = dynamic_cast<const TypeAliasDeclarationNode&>(*declarations.at(3));
        const auto* originalType = result.getTypeByTypeReferenceNode(&originalAlias.getOriginalType());
        const auto* optionalType = dynamic_cast<const CustomizedType*>(result.getTypeByTypeReferenceNode(&firstAlias.getOriginalType()));
        ASSERT_NE(originalType, nullptr);
        ASSERT_NE(optionalType, nullptr);
        EXPECT_NE(originalType, optionalType);
        EXPECT_EQ(result.getTypeByTypeReferenceNode(&secondAlias.getOriginalType()), optionalType);
        ASSERT_EQ(optionalType->getGenericArguments().size(), 1);
        EXPECT_EQ(optionalType->getGenericArguments().front(), originalType);
        EXPECT_EQ(result.getCustomizedTypeByFullTypeName(std::string(originalType->getFullTypeName())), originalType);
        EXPECT_EQ(result.getCustomizedTypeByFullTypeName(std::string(optionalType->getFullTypeName())), optionalType);
    }

    TEST_F(SemanticAnalyzerImportTest, DoesNotRegisterOptionalTypesWithUnresolvedOriginalTypes) {
        const auto result = analyze(
            "import vanillang.typesystem.Optional\nclass Box<T> {}\ntype MissingOptional = Missing?\ntype MissingArgument = Box<Missing?>\ntype MissingOptionalArgument = Box<Missing?>?\n"
        );
        ASSERT_EQ(result.getErrors().size(), 3);
        for (const auto& error : result.getErrors()) {
            EXPECT_EQ(error.getMessage(), "Use of undeclared type 'Missing'");
        }
        for (std::size_t index = 1; index < module->getTopIdentifierDeclarations().size(); ++index) {
            const auto& alias = dynamic_cast<const TypeAliasDeclarationNode&>(*module->getTopIdentifierDeclarations()[index]);
            EXPECT_EQ(result.getTypeByTypeReferenceNode(&alias.getOriginalType()), nullptr);
        }
        const auto boxName = std::string(module->getFullName()) + ".Box<vanillang.typesystem.Optional<Missing>>";
        EXPECT_EQ(result.getCustomizedTypeByFullTypeName("vanillang.typesystem.Optional<Missing>"), nullptr);
        EXPECT_EQ(result.getCustomizedTypeByFullTypeName(boxName), nullptr);
        EXPECT_EQ(result.getCustomizedTypeByFullTypeName("vanillang.typesystem.Optional<" + boxName + ">"), nullptr);
    }

    TEST_F(SemanticAnalyzerImportTest, ReusesOptionalTypesAcrossSugarAndExplicitReferencesInEitherOrder) {
        for (const std::string_view firstType : { "int?", "Optional<int>" }) {
            SCOPED_TRACE(firstType);
            const std::string secondType = firstType == "int?" ? "Optional<int>" : "int?";
            withAliasType(
                "import vanillang.typesystem.Optional\ntype First = " + std::string(firstType) + "\ntype Second = " + secondType + "\n",
                [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& secondAlias) {
                    auto& context = semanticContext(analyzer);
                    ASSERT_TRUE(context.getErrors().empty());
                    const auto& firstAlias = dynamic_cast<const TypeAliasDeclarationNode&>(*module->getTopIdentifierDeclarations().front());
                    const auto* optionalType = dynamic_cast<const CustomizedType*>(context.getTypeByTypeReferenceNode(&firstAlias.getOriginalType()));
                    ASSERT_NE(optionalType, nullptr);
                    EXPECT_EQ(optionalType->getCustomizedKind(), CustomizedTypeKind::ENUM);
                    EXPECT_EQ(optionalType->getFullTypeName(), "vanillang.typesystem.Optional<int>");
                    EXPECT_EQ(context.getTypeByTypeReferenceNode(&secondAlias.getOriginalType()), optionalType);
                    EXPECT_EQ(context.getCustomizedTypeByFullTypeName("vanillang.typesystem.Optional<int>"), optionalType);
                    ASSERT_EQ(optionalType->getGenericArguments().size(), 1);
                    EXPECT_EQ(optionalType->getGenericArguments().front(), PrimitiveType::intType());
                }
            );
        }
    }

    TEST_F(SemanticAnalyzerImportTest, RegistersLocalCustomizedTypesAndReusesExistingPointers) {
        withAliasType("class Box<T> {}\ntype Result = Box<int>\n", [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
            auto& context = semanticContext(analyzer);
            const auto* resolvedType = context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType());
            const auto* customizedType = dynamic_cast<const CustomizedType*>(resolvedType);
            ASSERT_NE(customizedType, nullptr);
            EXPECT_EQ(customizedType->getCustomizedKind(), CustomizedTypeKind::CLASS);
            EXPECT_EQ(customizedType->getOrigin(), CustomizedTypeOrigin::LOCAL);
            EXPECT_EQ(customizedType->getFullTypeName(), std::string(module->getFullName()) + ".Box<int>");
            ASSERT_EQ(customizedType->getGenericArguments().size(), 1);
            EXPECT_EQ(customizedType->getGenericArguments().front(), PrimitiveType::intType());
            EXPECT_EQ(customizedType->getLocalNode(), module->getTopIdentifierDeclarations().front().get());
            EXPECT_EQ(context.getCustomizedTypeByFullTypeName(std::string(customizedType->getFullTypeName())), customizedType);
            EXPECT_EQ(checkedType(analyzer, typeAlias.getOriginalType()), customizedType);
        });
    }

    TEST_F(SemanticAnalyzerImportTest, ReusesCustomizedTypesAcrossDifferentTypeReferenceNodes) {
        withAliasType("class Box<T> {}\ntype First = Box<int>\ntype Second = Box<int>\n", [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& secondAlias) {
            const TypeAliasDeclarationNode* firstAlias = nullptr;
            for (const auto& declaration : module->getTopIdentifierDeclarations()) {
                const auto* typeAlias = dynamic_cast<const TypeAliasDeclarationNode*>(declaration.get());
                if (typeAlias != nullptr && typeAlias->getAliasName().getIdentifierString() == "First") {
                    firstAlias = typeAlias;
                    break;
                }
            }
            ASSERT_NE(firstAlias, nullptr);
            auto& context = semanticContext(analyzer);
            EXPECT_EQ(context.getTypeByTypeReferenceNode(&firstAlias->getOriginalType()), context.getTypeByTypeReferenceNode(&secondAlias.getOriginalType()));
        });
    }

    TEST_F(SemanticAnalyzerImportTest, RegistersAndReusesImportedCustomizedTypes) {
        writeScopedModule();
        withAliasType("import pkg.api.Box as ImportedBox\ntype Result = ImportedBox<int>\n", [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
            auto& context = semanticContext(analyzer);
            const auto* resolvedType = context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType());
            const auto* customizedType = dynamic_cast<const CustomizedType*>(resolvedType);
            ASSERT_NE(customizedType, nullptr);
            EXPECT_EQ(customizedType->getCustomizedKind(), CustomizedTypeKind::CLASS);
            EXPECT_EQ(customizedType->getOrigin(), CustomizedTypeOrigin::IMPORTED);
            EXPECT_EQ(customizedType->getFullTypeName(), "pkg.api.Box<int>");
            ASSERT_EQ(customizedType->getGenericArguments().size(), 1);
            EXPECT_EQ(customizedType->getGenericArguments().front(), PrimitiveType::intType());
            const auto* moduleScope = context.currentScope().findParent();
            ASSERT_NE(moduleScope, nullptr);
            const auto* symbol = asRegularSymbol(moduleScope->lookupLocal("ImportedBox"));
            ASSERT_NE(symbol, nullptr);
            EXPECT_EQ(customizedType->getImportedNode(), symbol->getImportedNode());
            EXPECT_EQ(checkedType(analyzer, typeAlias.getOriginalType()), customizedType);
        });
    }

    TEST_F(SemanticAnalyzerImportTest, SubstitutesGenericArgumentsInImportedAliasTypeNames) {
        writeFile(
            "dependency_source/api.vni",
            R"({
    "Box": {"category": "class", "genericParameters": ["T"], "properties": {}, "methods": {}, "constructors": {}, "operators": {},
        "baseClass": null, "implementedInterfaces": [], "final": false},
    "Wrapped": {"category": "typealias", "genericParameters": ["T"], "originalType": "pkg.api.Box<T>"}
})"
        );

        withAliasType("import pkg.api.Wrapped\ntype Result = Wrapped<int>\n", [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
            auto& context = semanticContext(analyzer);
            ASSERT_TRUE(context.getErrors().empty()) << context.getErrors().front().getMessage();
            const auto* customizedType = dynamic_cast<const CustomizedType*>(context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType()));
            ASSERT_NE(customizedType, nullptr);
            EXPECT_EQ(customizedType->getFullTypeName(), "pkg.api.Box<int>");
            EXPECT_EQ(customizedType->getOrigin(), CustomizedTypeOrigin::IMPORTED);
            ASSERT_EQ(customizedType->getGenericArguments().size(), 1);
            EXPECT_EQ(customizedType->getGenericArguments().front(), PrimitiveType::intType());
        });
    }

    TEST_F(SemanticAnalyzerImportTest, SubstitutesGenericArgumentsInPrefixedImportedAliasTypeNames) {
        writeFile("dependency_source/api.vni", R"({"Identity":{"category":"typealias","genericParameters":["T"],"originalType":"T"}})");

        withAliasType(
            "import pkg.api as api\n"
            "class Box<T> {}\n"
            "type Wrapper<T> = Box<api.Identity<T>>\n"
            "type Result = Wrapper<int>\n",
            [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
                auto& context = semanticContext(analyzer);
                ASSERT_TRUE(context.getErrors().empty()) << context.getErrors().front().getMessage();
                const auto* customizedType = dynamic_cast<const CustomizedType*>(context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType()));
                ASSERT_NE(customizedType, nullptr);
                EXPECT_EQ(customizedType->getFullTypeName(), std::string(module->getFullName()) + ".Box<int>");
                ASSERT_EQ(customizedType->getGenericArguments().size(), 1);
                EXPECT_EQ(customizedType->getGenericArguments().front(), PrimitiveType::intType());
            }
        );
    }

    TEST_F(SemanticAnalyzerImportTest, AcceptsInheritanceFromImportedNonFinalClassesAndInterfaces) {
        writeInheritanceModule();
        const auto result = analyze("import pkg.api.{Base, Readable}\nclass Derived extends Base implements Readable {}\n");

        ASSERT_FALSE(result.hasErrors()) << result.getErrors().front().getMessage();
    }

    TEST_F(SemanticAnalyzerImportTest, RejectsInheritanceFromImportedFinalClassesAndNonClassTypes) {
        writeInheritanceModule();
        const std::vector<std::pair<std::string_view, std::string_view>> cases = {
            { "import pkg.api.Sealed\nclass Derived extends Sealed {}\n", "Inheritance from final classes is not allowed" },
            { "import pkg.api.Readable\nclass Derived extends Readable {}\n", "Inheritance from non-class types is not allowed" },
            { "import pkg.api.State\nclass Derived extends State {}\n", "Inheritance from non-class types is not allowed" },
            { "import pkg.api.Base\nclass Derived implements Base {}\n", "Type 'pkg.api.Base' is not an interface" },
            { "import pkg.api.State\nclass Derived implements State {}\n", "Type 'pkg.api.State' is not an interface" },
        };
        for (const auto& [source, message] : cases) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_EQ(result.getErrors().size(), 1);
            EXPECT_EQ(result.getErrors().front().getMessage(), message);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, AcceptsEqualityAndComparisonOperatorsWithRequiredReturnTypes) {
        const auto result = analyze(R"(
import vanillang.typesystem.{Optional, Ordering}
class Vec3 {
    op ==(other: Vec3) -> bool { return true }
    op <=>(other: Vec3) -> Ordering? { return none }
    native op ==(other: int) -> bool
    native op <=>(other: int) -> Optional<Ordering>
    op ==(other: string) { return true }
    op <=>(other: string) { return none }
}
interface Comparable {
    op ==(other: Comparable) -> bool
    op <=>(other: Comparable) -> Ordering?
}
)");

        ASSERT_FALSE(result.hasErrors()) << result.getErrors().front().getMessage();
        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(module->getTopIdentifierDeclarations().front().get());
        ASSERT_NE(classDeclaration, nullptr);
        ASSERT_EQ(classDeclaration->getOperatorDeclarations().size(), 6);

        const std::string vec3TypeName = std::string(module->getFullName()) + ".Vec3";
        EXPECT_EQ(classDeclaration->getOperatorDeclarations()[1]->getInternalName(), "vnl-operator-comparison(" + vec3TypeName + ")");
    }

    TEST_F(SemanticAnalyzerImportTest, RejectsComparisonOperatorsNotReturningOptionalOrdering) {
        const auto result = analyze(R"(
import vanillang.typesystem.{Optional, Ordering}
class Vec3 {
    op <=>(other: Vec3) -> int { return 0 }
    native op <=>(other: int) -> Ordering
    native op <=>(other: string) -> bool?
    native op <=>(other: bool)
}
interface Comparable {
    op <=>(other: Comparable) -> Optional<int>
    op <=>(other: int)
}
)");

        ASSERT_EQ(result.getErrors().size(), 6);
        for (const auto& error : result.getErrors()) {
            EXPECT_EQ(error.getMessage(), "Operator 'comparison' must return 'vanillang.typesystem.Optional<vanillang.typesystem.Ordering>'");
        }
    }

    TEST_F(SemanticAnalyzerImportTest, RejectsComparisonOperatorsReturningLocalOrderingTypes) {
        const auto result = analyze(R"(
import vanillang.typesystem.Optional
enum Ordering {
    Less
    Equal
    Greater
}
class Vec3 {
    native op <=>(other: Vec3) -> Ordering?
}
)");

        ASSERT_EQ(result.getErrors().size(), 1);
        EXPECT_EQ(result.getErrors().front().getMessage(), "Operator 'comparison' must return 'vanillang.typesystem.Optional<vanillang.typesystem.Ordering>'");
    }

    TEST_F(SemanticAnalyzerImportTest, BuildsImportedPackageModuleAndMemberScopes) {
        writeScopedModule();
        const auto result = analyze("import pkg as library\nlet localOnly = 0\nexport library\n");

        ASSERT_FALSE(result.hasErrors());
        const auto* localScope = result.getScopeByAstNode(*module);
        ASSERT_NE(localScope, nullptr);
        EXPECT_EQ(localScope->getOrigin(), ScopeOrigin::LOCAL);
        const auto* package = getImportedRootPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* packageScope = result.getScopeByImportedNode(*package);
        ASSERT_NE(packageScope, nullptr);
        EXPECT_EQ(packageScope->getKind(), ScopeKind::PACKAGE);
        EXPECT_EQ(packageScope->getOrigin(), ScopeOrigin::IMPORTED);
        EXPECT_EQ(packageScope->getImportedNode(), package);
        EXPECT_EQ(packageScope->getLocalNode(), nullptr);
        EXPECT_EQ(packageScope->findParent(), nullptr);
        EXPECT_EQ(findImportedNode(result, "library"), package);

        const auto* api = package->getModuleByName("api");
        ASSERT_NE(api, nullptr);
        const auto* apiScope = result.getScopeByImportedNode(*api);
        ASSERT_NE(apiScope, nullptr);
        EXPECT_EQ(apiScope->getKind(), ScopeKind::MODULE);
        EXPECT_EQ(apiScope->findParent(), packageScope);
        const auto* apiSymbol = asRegularSymbol(packageScope->lookupLocal("api"));
        ASSERT_NE(apiSymbol, nullptr);
        EXPECT_EQ(apiSymbol->getKind(), RegularSymbolKind::MODULE);
        EXPECT_EQ(apiSymbol->getImportedNode(), api);

        const auto expectChildScope = [&](const Scope& parent, std::string_view name, ScopeKind kind) -> const Scope* {
            const Symbol* symbol = parent.lookupLocal(name);
            EXPECT_NE(symbol, nullptr);
            if (symbol == nullptr) return nullptr;
            const ImportedItem* node = nullptr;
            if (const auto* regularSymbol = dynamic_cast<const RegularSymbol*>(symbol)) {
                node = regularSymbol->getImportedNode();
            } else if (const auto* functionSymbol = dynamic_cast<const FunctionSymbol*>(symbol); functionSymbol != nullptr && functionSymbol->isUnique()) {
                node = functionSymbol->getOverloadings().begin()->second.getImportedNode();
            }
            EXPECT_NE(node, nullptr);
            if (node == nullptr) return nullptr;
            const auto* scope = result.getScopeByImportedNode(*node);
            EXPECT_NE(scope, nullptr);
            if (scope != nullptr) {
                EXPECT_EQ(scope->getKind(), kind);
                EXPECT_EQ(scope->getOrigin(), ScopeOrigin::IMPORTED);
                EXPECT_EQ(scope->getImportedNode(), node);
                EXPECT_EQ(scope->getLocalNode(), nullptr);
                EXPECT_EQ(scope->findParent(), &parent);
            }
            return scope;
        };
        const auto* subScope = expectChildScope(*packageScope, "sub", ScopeKind::PACKAGE);
        ASSERT_NE(subScope, nullptr);
        EXPECT_NE(expectChildScope(*subScope, "other", ScopeKind::MODULE), nullptr);

        const auto* boxScope = expectChildScope(*apiScope, "Box", ScopeKind::CLASS);
        ASSERT_NE(boxScope, nullptr);
        const auto* visible = asRegularSymbol(boxScope->lookupLocal("visible"));
        const auto* secret = asRegularSymbol(boxScope->lookupLocal("secret"));
        const auto* method = dynamic_cast<const FunctionSymbol*>(boxScope->lookupLocal("apply"));
        ASSERT_NE(visible, nullptr);
        ASSERT_NE(secret, nullptr);
        ASSERT_NE(method, nullptr);
        ASSERT_TRUE(method->isUnique());
        EXPECT_EQ(visible->getKind(), RegularSymbolKind::PROPERTY);
        EXPECT_EQ(visible->getAccessModifier(), RegularSymbolAccessModifier::PUBLIC);
        EXPECT_EQ(secret->getAccessModifier(), RegularSymbolAccessModifier::PRIVATE);
        EXPECT_EQ(method->getName(), "apply");
        const auto& methodOverloading = method->getOverloadings().begin()->second;
        EXPECT_EQ(methodOverloading.getKind(), RegularSymbolKind::METHOD);
        EXPECT_EQ(methodOverloading.getAccessModifier(), RegularSymbolAccessModifier::PROTECTED);
        EXPECT_EQ(result.getScopeByImportedNode(*visible->getImportedNode()), nullptr);
        const auto* methodScope = expectChildScope(*boxScope, "apply", ScopeKind::FUNCTION);
        ASSERT_NE(methodScope, nullptr);
        const auto* input = asRegularSymbol(methodScope->lookupLocal("input"));
        ASSERT_NE(input, nullptr);
        EXPECT_EQ(input->getKind(), RegularSymbolKind::PARAMETER);
        EXPECT_EQ(input->getOrigin(), RegularSymbolOrigin::IMPORTED);
        EXPECT_EQ(result.getScopeByImportedNode(*input->getImportedNode()), nullptr);
        const auto* generic = asRegularSymbol(boxScope->lookupLocal("T"));
        ASSERT_NE(generic, nullptr);
        EXPECT_EQ(generic->getKind(), RegularSymbolKind::GENERIC_PARAMETER);
        EXPECT_EQ(generic->getImportedNode(), boxScope->getImportedNode());
        EXPECT_EQ(methodScope->lookup("T"), generic);
        EXPECT_EQ(methodScope->lookup("localOnly"), nullptr);

        const auto* interfaceScope = expectChildScope(*apiScope, "Readable", ScopeKind::INTERFACE);
        ASSERT_NE(interfaceScope, nullptr);
        const auto* readScope = expectChildScope(*interfaceScope, "read", ScopeKind::FUNCTION);
        ASSERT_NE(readScope, nullptr);
        EXPECT_NE(readScope->lookupLocal("input"), nullptr);
        EXPECT_NE(interfaceScope->lookupLocal("R"), nullptr);
        EXPECT_EQ(readScope->lookup("R"), interfaceScope->lookupLocal("R"));

        const auto* enumScope = expectChildScope(*apiScope, "State", ScopeKind::ENUM);
        ASSERT_NE(enumScope, nullptr);
        const auto* memberScope = expectChildScope(*enumScope, "Ready", ScopeKind::ENUM_MEMBER);
        ASSERT_NE(memberScope, nullptr);
        const auto* payload = asRegularSymbol(memberScope->lookupLocal("payload"));
        ASSERT_NE(payload, nullptr);
        EXPECT_EQ(payload->getKind(), RegularSymbolKind::PROPERTY);
        EXPECT_EQ(payload->getOrigin(), RegularSymbolOrigin::IMPORTED);
        EXPECT_EQ(result.getScopeByImportedNode(*payload->getImportedNode()), nullptr);
        EXPECT_NE(enumScope->lookupLocal("E"), nullptr);
        EXPECT_EQ(memberScope->lookup("E"), enumScope->lookupLocal("E"));

        const auto* aliasScope = expectChildScope(*apiScope, "Alias", ScopeKind::TYPE_ALIAS);
        ASSERT_NE(aliasScope, nullptr);
        EXPECT_NE(aliasScope->lookupLocal("A"), nullptr);
        const auto* functionScope = expectChildScope(*apiScope, "run", ScopeKind::FUNCTION);
        ASSERT_NE(functionScope, nullptr);
        EXPECT_NE(functionScope->lookupLocal("amount"), nullptr);
        const auto* external = asRegularSymbol(apiScope->lookupLocal("External"));
        ASSERT_NE(external, nullptr);
        EXPECT_EQ(external->getKind(), RegularSymbolKind::IMPORT_ALIAS);
        EXPECT_EQ(result.getScopeByImportedNode(*external->getImportedNode()), nullptr);
        const auto* extraPackage = getImportedRootPackageByName("extra");
        ASSERT_NE(extraPackage, nullptr);
        EXPECT_EQ(result.getScopeByImportedNode(*extraPackage), nullptr);
        for (const auto name : { "api", "Box", "T", "apply", "input", "Ready", "payload", "extra", "External" }) {
            EXPECT_EQ(localScope->lookupLocal(name), nullptr);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, CreatesScopesForDirectAndWildcardImportsOfScopedIdentifiers) {
        writeScopedModule();
        struct ScopedIdentifier {
            std::string path;
            std::string name;
            ScopeKind kind;
        };
        const std::vector<ScopedIdentifier> scopedIdentifiers = {
            { "Box", "Box", ScopeKind::CLASS },          { "Readable", "Readable", ScopeKind::INTERFACE }, { "State", "State", ScopeKind::ENUM },
            { "Alias", "Alias", ScopeKind::TYPE_ALIAS }, { "method", "method", ScopeKind::FUNCTION },      { "Ready", "Ready", ScopeKind::ENUM_MEMBER },
        };
        for (const auto& [path, name, kind] : scopedIdentifiers) {
            SCOPED_TRACE(path);
            const auto result = analyze("import pkg.api." + path + " as selected\nexport selected\n");
            ASSERT_FALSE(result.hasErrors());
            const auto* node = findImportedNode(result, "selected");
            ASSERT_NE(node, nullptr);
            const auto* scope = result.getScopeByImportedNode(*node);
            ASSERT_NE(scope, nullptr);
            EXPECT_EQ(scope->getKind(), kind);
            EXPECT_EQ(scope->getImportedNode(), node);
            const auto* localScope = result.getScopeByAstNode(*module);
            ASSERT_NE(localScope, nullptr);
            EXPECT_EQ(localScope->lookupLocal(name), nullptr);
            EXPECT_EQ(localScope->lookupLocal("api"), nullptr);
            EXPECT_EQ(localScope->lookupLocal("pkg"), nullptr);
        }
        const auto result = analyze("import pkg.api.*\n");
        ASSERT_FALSE(result.hasErrors());
        const std::vector<std::pair<std::string, ScopeKind>> wildcardScopedIdentifiers = {
            { "run", ScopeKind::FUNCTION },     { "Box", ScopeKind::CLASS },       { "Readable", ScopeKind::INTERFACE }, { "State", ScopeKind::ENUM },
            { "Alias", ScopeKind::TYPE_ALIAS }, { "method", ScopeKind::FUNCTION }, { "Ready", ScopeKind::ENUM_MEMBER },
        };
        for (const auto& [name, kind] : wildcardScopedIdentifiers) {
            const auto* node = findImportedNode(result, name);
            ASSERT_NE(node, nullptr);
            const auto* scope = result.getScopeByImportedNode(*node);
            ASSERT_NE(scope, nullptr);
            EXPECT_EQ(scope->getKind(), kind);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, DoesNotCreateScopesForImportedValuesOrAliasesToValues) {
        writeScopedModule();
        for (const auto name : { "value", "property", "parameter", "External" }) {
            SCOPED_TRACE(name);
            const auto result = analyze(std::string("import pkg.api.") + name + "\n");
            ASSERT_FALSE(result.hasErrors());
            const auto* node = findImportedNode(result, name);
            ASSERT_NE(node, nullptr);
            EXPECT_EQ(result.getScopeByImportedNode(*node), nullptr);
            const auto* package = getImportedRootPackageByName("pkg");
            ASSERT_NE(package, nullptr);
            EXPECT_EQ(result.getScopeByImportedNode(*package), nullptr);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, ReusesImportedScopesAcrossAliasesAndImportOrders) {
        writeScopedModule();
        for (const auto source : {
                 "import pkg.api.Box as First\nimport pkg.api as api\nimport pkg as library\nimport pkg.api.Box as Second\nimport extra.tools as tools\n",
                 "import pkg as library\nimport pkg.api as api\nimport pkg.api.Box as First\nimport pkg.api.Box as Second\nimport extra.tools as tools\n",
                 "import pkg.api as api\nimport pkg.api.Box as First\nimport pkg as library\nimport pkg.api.Box as Second\nimport extra.tools as tools\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);
            ASSERT_FALSE(result.hasErrors());
            const auto* first = findImportedNode(result, "First");
            const auto* second = findImportedNode(result, "Second");
            const auto* api = findImportedNode(result, "api");
            const auto* library = findImportedNode(result, "library");
            const auto* tools = findImportedNode(result, "tools");
            ASSERT_NE(first, nullptr);
            ASSERT_EQ(first, second);
            ASSERT_NE(api, nullptr);
            ASSERT_NE(library, nullptr);
            ASSERT_NE(tools, nullptr);
            const auto* classScope = result.getScopeByImportedNode(*first);
            const auto* apiScope = result.getScopeByImportedNode(*api);
            const auto* packageScope = result.getScopeByImportedNode(*library);
            ASSERT_NE(classScope, nullptr);
            ASSERT_NE(apiScope, nullptr);
            ASSERT_NE(packageScope, nullptr);
            EXPECT_EQ(classScope, result.getScopeByImportedNode(*second));
            EXPECT_EQ(classScope->findParent(), apiScope);
            EXPECT_EQ(apiScope->findParent(), packageScope);
            const auto* method = dynamic_cast<const FunctionSymbol*>(classScope->lookupLocal("apply"));
            ASSERT_NE(method, nullptr);
            ASSERT_TRUE(method->isUnique());
            const auto* methodOverloading = &method->getOverloadings().begin()->second;
            const auto* methodScope = result.getScopeByImportedNode(*methodOverloading->getImportedNode());
            ASSERT_NE(methodScope, nullptr);
            EXPECT_EQ(methodScope->findParent(), classScope);
            EXPECT_EQ(methodScope->lookup("T"), classScope->lookupLocal("T"));
            const auto* toolsScope = result.getScopeByImportedNode(*tools);
            ASSERT_NE(toolsScope, nullptr);
            EXPECT_NE(toolsScope->lookupLocal("enabled"), nullptr);
            EXPECT_NE(toolsScope, apiScope);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, DeclaresScopesForModulesAcrossSeparateImports) {
        const auto result = analyze("import pkg.api as api\nimport pkg.sub.other.flag\n");
        ASSERT_FALSE(result.hasErrors());
        const auto* package = getImportedRootPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* packageScope = result.getScopeByImportedNode(*package);
        ASSERT_NE(packageScope, nullptr);
        const auto* sub = asRegularSymbol(packageScope->lookupLocal("sub"));
        ASSERT_NE(sub, nullptr);
        const auto* subScope = result.getScopeByImportedNode(*sub->getImportedNode());
        ASSERT_NE(subScope, nullptr);
        EXPECT_EQ(subScope->findParent(), packageScope);
        const auto* other = asRegularSymbol(subScope->lookupLocal("other"));
        ASSERT_NE(other, nullptr);
        const auto* otherScope = result.getScopeByImportedNode(*other->getImportedNode());
        ASSERT_NE(otherScope, nullptr);
        const auto* flag = asRegularSymbol(otherScope->lookupLocal("flag"));
        ASSERT_NE(flag, nullptr);
        EXPECT_EQ(flag->getImportedNode(), findImportedNode(result, "flag"));
        EXPECT_EQ(otherScope->findParent(), subScope);
    }

    TEST_F(SemanticAnalyzerImportTest, DeclaresScopesForIndirectModulesInAnImportedPackage) {
        writeFile("dependency_source/api.vni", R"({"value":{"category":"let","type":"extra.types.Remote"}})");
        writeFile("another_source/types.vni", R"({"Remote":{"category":"typealias","genericParameters":["T"],"originalType":"T"}})");
        const auto result = analyze("import extra.tools as tools\nimport pkg.api.value\n");

        ASSERT_FALSE(result.hasErrors());
        const auto* package = getImportedRootPackageByName("extra");
        ASSERT_NE(package, nullptr);
        const auto* packageScope = result.getScopeByImportedNode(*package);
        ASSERT_NE(packageScope, nullptr);
        const auto* types = asRegularSymbol(packageScope->lookupLocal("types"));
        ASSERT_NE(types, nullptr);
        const auto* typesScope = result.getScopeByImportedNode(*types->getImportedNode());
        ASSERT_NE(typesScope, nullptr);
        EXPECT_EQ(typesScope->findParent(), packageScope);
        const auto* remote = asRegularSymbol(typesScope->lookupLocal("Remote"));
        ASSERT_NE(remote, nullptr);
        const auto* remoteScope = result.getScopeByImportedNode(*remote->getImportedNode());
        ASSERT_NE(remoteScope, nullptr);
        EXPECT_EQ(remoteScope->findParent(), typesScope);
        EXPECT_NE(remoteScope->lookupLocal("T"), nullptr);
        const auto* tools = findImportedNode(result, "tools");
        ASSERT_NE(tools, nullptr);
        const auto* toolsScope = result.getScopeByImportedNode(*tools);
        ASSERT_NE(toolsScope, nullptr);
        EXPECT_EQ(toolsScope->findParent(), packageScope);
        const auto* localScope = result.getScopeByAstNode(*module);
        ASSERT_NE(localScope, nullptr);
        EXPECT_EQ(localScope->lookupLocal("types"), nullptr);
        EXPECT_EQ(localScope->lookupLocal("Remote"), nullptr);
    }

    TEST_F(SemanticAnalyzerImportTest, DeclaresOnlyTheTerminalMember) {
        const auto result = analyze("import pkg.api.value\nlet pkg = 0\nlet api = 0\nexport value\n");

        ASSERT_FALSE(result.hasErrors());
        EXPECT_EQ(findImportedNode(result, "pkg"), nullptr);
        EXPECT_EQ(findImportedNode(result, "api"), nullptr);
        const auto* package = getImportedRootPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* importedModule = package->getModuleByName("api");
        ASSERT_NE(importedModule, nullptr);
        const auto* scope = result.getScopeByAstNode(*module);
        ASSERT_NE(scope, nullptr);
        EXPECT_EQ(scope->lookupLocal("count"), nullptr);
        const auto* member = importedModule->getIdentifierByName("value");
        ASSERT_NE(member, nullptr);
        EXPECT_EQ(findImportedNode(result, "value"), member);
    }

    TEST_F(SemanticAnalyzerImportTest, DoesNotDeclarePathPrefixesInTheModuleScope) {
        const auto result = analyze("import pkg.api.value\nexport pkg, api\n");

        ASSERT_TRUE(result.hasErrors());
        ASSERT_EQ(result.getErrors().size(), 2);
        EXPECT_EQ(result.getErrors()[0].getMessage(), "Use of undeclared identifier pkg");
        EXPECT_EQ(result.getErrors()[1].getMessage(), "Use of undeclared identifier api");
    }

    TEST_F(SemanticAnalyzerImportTest, DeclaresOnlyTheTerminalModule) {
        const auto result = analyze("import pkg.api\nlet pkg = 0\nlet value = 0\nexport api\n");

        ASSERT_FALSE(result.hasErrors());
        const auto* package = getImportedRootPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* importedModule = package->getModuleByName("api");
        ASSERT_NE(importedModule, nullptr);
        EXPECT_EQ(findImportedNode(result, "api"), importedModule);
        EXPECT_EQ(findImportedNode(result, "pkg"), nullptr);
        EXPECT_EQ(findImportedNode(result, "value"), nullptr);
        const auto* scope = result.getScopeByAstNode(*module);
        ASSERT_NE(scope, nullptr);
        EXPECT_EQ(scope->lookupLocal("count"), nullptr);
    }

    TEST_F(SemanticAnalyzerImportTest, DeclaresImportedSymbolsWithTheirKindsAndOriginalTargets) {
        writeFile(
            "dependency_source/api.vni",
            R"({
    "value": {"category": "let", "type": "int"},
    "run": {"category": "func", "name": "run", "returnType": "void", "parameters": {}, "native": false},
    "Box": {"category": "class", "genericParameters": [], "properties": {}, "methods": {}, "constructors": {}, "operators": {}, "baseClass": null, "implementedInterfaces": [], "final": false},
    "Readable": {"category": "interface", "genericParameters": [], "methods": {}, "operators": {}},
    "State": {"category": "enum", "genericParameters": [], "members": {}},
    "Alias": {"category": "typealias", "genericParameters": [], "originalType": "int"},
    "External": {"category": "imported", "source": "extra.tools.enabled"},
    "method": {"category": "method", "name": "method", "returnType": "void", "parameters": {}, "native": false, "static": true, "accessModifier": "public"},
    "Ready": {"category": "enummember", "associatedValues": {}},
    "property": {"category": "property", "type": "int", "static": true, "accessModifier": "public"},
    "parameter": {"category": "parameter", "type": "int"}
})"
        );
        struct ImportedIdentifier {
            std::string path;
            std::string name;
            RegularSymbolKind kind;
            bool function;
        };
        const std::vector<ImportedIdentifier> identifiers = {
            { "value", "value", RegularSymbolKind::VARIABLE, false },
            { "run", "run", RegularSymbolKind::FUNCTION, true },
            { "Box", "Box", RegularSymbolKind::CLASS, false },
            { "Readable", "Readable", RegularSymbolKind::INTERFACE, false },
            { "State", "State", RegularSymbolKind::ENUM, false },
            { "Alias", "Alias", RegularSymbolKind::TYPE_ALIAS, false },
            { "External", "External", RegularSymbolKind::VARIABLE, false },
            { "method", "method", RegularSymbolKind::METHOD, true },
            { "Ready", "Ready", RegularSymbolKind::ENUM_MEMBER, false },
            { "property", "property", RegularSymbolKind::PROPERTY, false },
            { "parameter", "parameter", RegularSymbolKind::PARAMETER, false },
        };

        for (const auto& [path, name, kind, function] : identifiers) {
            for (const bool aliased : { false, true }) {
                const auto source = "import pkg.api." + path + (aliased ? " as selected\n" : "\n");
                SCOPED_TRACE(source);
                const auto result = analyze(source);

                ASSERT_FALSE(result.hasErrors());
                const auto* package = getImportedRootPackageByName("pkg");
                ASSERT_NE(package, nullptr);
                const auto* importedModule = package->getModuleByName("api");
                ASSERT_NE(importedModule, nullptr);
                const auto* scope = result.getScopeByAstNode(*module);
                ASSERT_NE(scope, nullptr);
                EXPECT_EQ(scope->lookupLocal("pkg"), nullptr);
                EXPECT_EQ(scope->lookupLocal("api"), nullptr);
                const Symbol* symbol = scope->lookupLocal(aliased ? "selected" : name);
                ASSERT_NE(symbol, nullptr);

                const ImportedItem* expectedTarget = nullptr;
                if (path == "External") {
                    const auto* extraPackage = getImportedRootPackageByName("extra");
                    ASSERT_NE(extraPackage, nullptr);
                    const auto* toolsModule = extraPackage->getModuleByName("tools");
                    ASSERT_NE(toolsModule, nullptr);
                    expectedTarget = toolsModule->getIdentifierByName("enabled");
                } else {
                    expectedTarget = importedModule->getIdentifierByName(path);
                }

                if (function) {
                    const auto* functionSymbol = dynamic_cast<const FunctionSymbol*>(symbol);
                    ASSERT_NE(functionSymbol, nullptr);
                    EXPECT_EQ(functionSymbol->getName(), aliased ? "selected" : name);
                    ASSERT_TRUE(functionSymbol->isUnique());
                    const auto& overloading = functionSymbol->getOverloadings().begin()->second;
                    EXPECT_EQ(overloading.getKind(), kind);
                    EXPECT_EQ(overloading.getOrigin(), RegularSymbolOrigin::IMPORTED);
                    EXPECT_EQ(overloading.getLocalNode(), nullptr);
                    EXPECT_EQ(overloading.getImportedNode(), expectedTarget);
                } else {
                    const auto* regularSymbol = dynamic_cast<const RegularSymbol*>(symbol);
                    ASSERT_NE(regularSymbol, nullptr);
                    EXPECT_EQ(regularSymbol->getName(), aliased ? "selected" : name);
                    EXPECT_EQ(regularSymbol->getKind(), kind);
                    EXPECT_EQ(regularSymbol->getOrigin(), RegularSymbolOrigin::IMPORTED);
                    EXPECT_EQ(regularSymbol->getLocalNode(), nullptr);
                    EXPECT_EQ(regularSymbol->getImportedNode(), expectedTarget);
                }
                for (const auto& otherIdentifier : identifiers) {
                    if (aliased || otherIdentifier.name != name) {
                        EXPECT_EQ(scope->lookupLocal(otherIdentifier.name), nullptr);
                    }
                }
            }
        }
    }

    TEST_F(SemanticAnalyzerImportTest, BindsRepeatedImportsToTheSameTargets) {
        for (const auto source : {
                 "import pkg.api.value\nimport pkg.api.count\nimport pkg.api.value as renamed\nimport pkg.api as library\nimport pkg as package\n",
                 "import pkg as package\nimport pkg.api as library\nimport pkg.api.value\nimport pkg.api.count\nimport pkg.api.value as renamed\n",
                 "import pkg.api as library\nimport pkg.api.value\nimport pkg.api.count\nimport pkg.api.value as renamed\nimport pkg as package\n",
                 "import pkg.{api.{value, count, value as renamed, self as library}, self as package}\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_FALSE(result.hasErrors());
            const auto* package = getImportedRootPackageByName("pkg");
            ASSERT_NE(package, nullptr);
            const auto* importedModule = package->getModuleByName("api");
            ASSERT_NE(importedModule, nullptr);
            const auto* value = importedModule->getIdentifierByName("value");
            const auto* count = importedModule->getIdentifierByName("count");
            ASSERT_NE(value, nullptr);
            ASSERT_NE(count, nullptr);
            EXPECT_EQ(findImportedNode(result, "value"), value);
            EXPECT_EQ(findImportedNode(result, "renamed"), value);
            EXPECT_EQ(findImportedNode(result, "count"), count);
            EXPECT_EQ(findImportedNode(result, "library"), importedModule);
            EXPECT_EQ(findImportedNode(result, "package"), package);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, AliasesBindToTheirOriginalTargets) {
        const auto result = analyze("import pkg.api.value as renamed\nimport pkg.api as library\nlet value = 0\nlet api = 0\nlet pkg = 0\nexport renamed, library\n");

        ASSERT_FALSE(result.hasErrors());
        const auto* package = getImportedRootPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* importedModule = package->getModuleByName("api");
        ASSERT_NE(importedModule, nullptr);
        EXPECT_EQ(findImportedNode(result, "renamed"), importedModule->getIdentifierByName("value"));
        EXPECT_EQ(findImportedNode(result, "library"), importedModule);
        EXPECT_EQ(findImportedNode(result, "value"), nullptr);
        EXPECT_EQ(findImportedNode(result, "api"), nullptr);
    }

    TEST_F(SemanticAnalyzerImportTest, ResolvesReexportedAliasesWhilePreservingDirectRenamedAndWildcardBindings) {
        writeFile("dependency_source/api.vni", R"({"External":{"category":"imported","source":"pkg.sub.other.bridge"}})");
        writeFile("dependency_source/sub/other.vni", R"({"bridge":{"category":"imported","source":"extra.tools.enabled"}})");

        for (const auto source : {
                 "import pkg.api.External\nimport pkg.api.External as renamed\nimport extra.tools.enabled\n",
                 "import pkg.api.*\nimport pkg.api.External as renamed\nimport extra.tools.enabled\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_FALSE(result.hasErrors());
            const auto* scope = result.getScopeByAstNode(*module);
            ASSERT_NE(scope, nullptr);
            const auto* enabled = findImportedNode(result, "enabled");
            ASSERT_NE(enabled, nullptr);
            EXPECT_EQ(enabled->getName(), "enabled");
            for (const auto name : { "External", "renamed", "enabled" }) {
                const auto* symbol = asRegularSymbol(scope->lookupLocal(name));
                ASSERT_NE(symbol, nullptr);
                EXPECT_EQ(symbol->getName(), name);
                EXPECT_EQ(symbol->getKind(), RegularSymbolKind::VARIABLE);
                EXPECT_EQ(symbol->getOrigin(), RegularSymbolOrigin::IMPORTED);
                EXPECT_EQ(symbol->getImportedNode(), enabled);
            }
            for (const auto name : { "pkg", "api", "bridge", "extra", "tools" }) {
                EXPECT_EQ(scope->lookupLocal(name), nullptr);
            }
        }
    }

    TEST_F(SemanticAnalyzerImportTest, ResolvesNestedImportsAndSelfAliases) {
        const auto result = analyze("import pkg.{api.{self as m, value as v}, sub.other}\nlet pkg = 0\nlet api = 0\nlet sub = 0\nlet value = 0\nexport m, v, other\n");

        ASSERT_FALSE(result.hasErrors());
        const auto* package = getImportedRootPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* importedModule = package->getModuleByName("api");
        ASSERT_NE(importedModule, nullptr);
        const auto* subPackage = package->getSubPackageByName("sub");
        ASSERT_NE(subPackage, nullptr);
        EXPECT_EQ(findImportedNode(result, "m"), importedModule);
        EXPECT_EQ(findImportedNode(result, "v"), importedModule->getIdentifierByName("value"));
        EXPECT_EQ(findImportedNode(result, "other"), subPackage->getModuleByName("other"));
        EXPECT_EQ(findImportedNode(result, "self"), nullptr);
    }

    TEST_F(SemanticAnalyzerImportTest, ExpandsModuleWildcardsAndCanImportTheModuleItself) {
        for (const auto source : { "import pkg.api.*\nlet api = 0\nexport value, count\n", "import pkg.api.{*, self}\nexport api, value, count\n" }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_FALSE(result.hasErrors());
            EXPECT_NE(findImportedNode(result, "value"), nullptr);
            EXPECT_NE(findImportedNode(result, "count"), nullptr);
            EXPECT_EQ(findImportedNode(result, "pkg"), nullptr);
            EXPECT_EQ(findImportedNode(result, "*"), nullptr);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, BindsPackagesAndPackageSelf) {
        for (const auto source : { "import pkg as library\nexport library\n", "import pkg.{self as library}\nexport library\n" }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_FALSE(result.hasErrors());
            const auto* package = getImportedRootPackageByName("pkg");
            ASSERT_NE(package, nullptr);
            EXPECT_EQ(findImportedNode(result, "library"), package);
            EXPECT_EQ(findImportedNode(result, "api"), nullptr);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, RetainsImportedBindingsAfterTheAnalyzerIsDestroyed) {
        const auto result = analyze("import pkg.api.value\nimport pkg.sub.other.flag\nimport pkg.sub.second.caption\nimport extra.tools.enabled\nexport value, flag, caption, enabled\n");

        ASSERT_FALSE(result.hasErrors());
        for (const auto name : { "value", "flag", "caption", "enabled" }) {
            const auto* binding = findImportedNode(result, name);
            ASSERT_NE(binding, nullptr);
            EXPECT_EQ(binding->getName(), name);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, DoesNotDeclareIndirectImportBindings) {
        writeFile("dependency_source/api.vni", R"({"value":{"category":"let","type":"int"},"External":{"category":"imported","source":"extra.tools.enabled"}})");

        const auto result = analyze("import pkg.api.value\nlet enabled = 0\nlet tools = 0\nlet extra = 0\nexport value, enabled, tools, extra\n");

        ASSERT_FALSE(result.hasErrors());
        const auto* package = getImportedRootPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* importedModule = package->getModuleByName("api");
        ASSERT_NE(importedModule, nullptr);
        EXPECT_EQ(findImportedNode(result, "value"), importedModule->getIdentifierByName("value"));
        const auto* scope = result.getScopeByAstNode(*module);
        ASSERT_NE(scope, nullptr);
        EXPECT_EQ(scope->lookupLocal("External"), nullptr);
        for (const auto name : { "enabled", "tools", "extra" }) {
            const auto* symbol = asRegularSymbol(scope->lookupLocal(name));
            ASSERT_NE(symbol, nullptr);
            EXPECT_NE(symbol->getLocalNode(), nullptr);
            EXPECT_EQ(symbol->getImportedNode(), nullptr);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, BindsReexportedNamesThroughRepeatedAndCyclicModuleDependencies) {
        writeFile("dependency_source/api.vni", R"({"External":{"category":"imported","source":"pkg.sub.other.bridge"},"repeated":{"category":"imported","source":"pkg.sub.other.bridge"}})");
        writeFile("dependency_source/sub/other.vni", R"({"bridge":{"category":"imported","source":"extra.tools.enabled"},"back":{"category":"imported","source":"pkg.api.External"}})");

        const auto result = analyze("import pkg.api.External\nexport External\n");

        ASSERT_FALSE(result.hasErrors());
        const auto* extraPackage = getImportedRootPackageByName("extra");
        ASSERT_NE(extraPackage, nullptr);
        const auto* toolsModule = extraPackage->getModuleByName("tools");
        ASSERT_NE(toolsModule, nullptr);
        EXPECT_EQ(findImportedNode(result, "External"), toolsModule->getIdentifierByName("enabled"));
        const auto* scope = result.getScopeByAstNode(*module);
        ASSERT_NE(scope, nullptr);
        const auto* external = asRegularSymbol(scope->lookupLocal("External"));
        ASSERT_NE(external, nullptr);
        EXPECT_EQ(external->getKind(), RegularSymbolKind::VARIABLE);
        for (const auto name : { "repeated", "bridge", "back", "enabled", "other", "tools" }) {
            EXPECT_EQ(scope->lookupLocal(name), nullptr);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, BindsReexportedPackagesAndModules) {
        for (const auto source : { "extra", "extra.tools", "pkg.sub" }) {
            SCOPED_TRACE(source);
            writeFile("dependency_source/api.vni", std::string(R"({"External":{"category":"imported","source":")") + source + R"("}})");

            const auto result = analyze("import pkg.api.External\nexport External\n");

            ASSERT_FALSE(result.hasErrors());
            const ImportedItem* target = nullptr;
            if (std::string_view(source) == "pkg.sub") {
                const auto* package = getImportedRootPackageByName("pkg");
                ASSERT_NE(package, nullptr);
                const auto* subPackage = package->getSubPackageByName("sub");
                ASSERT_NE(subPackage, nullptr);
                target = subPackage;
            } else {
                const auto* extraPackage = getImportedRootPackageByName("extra");
                ASSERT_NE(extraPackage, nullptr);
                target = std::string_view(source) == "extra" ? static_cast<const ImportedItem*>(extraPackage) : extraPackage->getModuleByName("tools");
            }
            const auto* scope = result.getScopeByAstNode(*module);
            ASSERT_NE(scope, nullptr);
            const auto* external = asRegularSymbol(scope->lookupLocal("External"));
            ASSERT_NE(external, nullptr);
            EXPECT_EQ(external->getImportedNode(), target);
            EXPECT_EQ(external->getKind(), std::string_view(source) == "extra.tools" ? RegularSymbolKind::MODULE : RegularSymbolKind::PACKAGE);
            ASSERT_NE(target, nullptr);
            const auto* targetScope = result.getScopeByImportedNode(*target);
            ASSERT_NE(targetScope, nullptr);
            EXPECT_EQ(targetScope->getKind(), std::string_view(source) == "extra.tools" ? ScopeKind::MODULE : ScopeKind::PACKAGE);
            if (std::string_view(source) == "extra") {
                EXPECT_EQ(targetScope->findParent(), nullptr);
            } else {
                const auto* parentPackage = getImportedRootPackageByName(std::string_view(source) == "pkg.sub" ? "pkg" : "extra");
                ASSERT_NE(parentPackage, nullptr);
                const auto* parentScope = result.getScopeByImportedNode(*parentPackage);
                ASSERT_NE(parentScope, nullptr);
                EXPECT_EQ(targetScope->findParent(), parentScope);
            }
            for (const auto name : { "extra", "tools", "sub", "other", "second" }) {
                EXPECT_EQ(scope->lookupLocal(name), nullptr);
            }
        }
    }

    TEST_F(SemanticAnalyzerImportTest, PreservesBindingsWhenDirectAndIndirectImportsShareModules) {
        writeFile("dependency_source/api.vni", R"({"External":{"category":"imported","source":"extra.tools.enabled"}})");
        writeFile("dependency_source/sub/other.vni", R"({"flag":{"category":"let","type":"bool"},"External":{"category":"imported","source":"extra.tools.enabled"}})");

        for (const auto source : {
                 "import pkg.api.External\nimport extra.tools.enabled\nimport pkg.sub.other.flag\nimport extra.tools as tools\nexport External, enabled, flag, tools\n",
                 "import extra.tools.enabled\nimport pkg.api.External\nimport pkg.sub.other.flag\nimport extra.tools as tools\nexport External, enabled, flag, tools\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_FALSE(result.hasErrors());
            const auto* package = getImportedRootPackageByName("pkg");
            ASSERT_NE(package, nullptr);
            const auto* subPackage = package->getSubPackageByName("sub");
            ASSERT_NE(subPackage, nullptr);
            const auto* otherModule = subPackage->getModuleByName("other");
            ASSERT_NE(otherModule, nullptr);
            EXPECT_EQ(findImportedNode(result, "flag"), otherModule->getIdentifierByName("flag"));
            const auto* extraPackage = getImportedRootPackageByName("extra");
            ASSERT_NE(extraPackage, nullptr);
            const auto* toolsModule = extraPackage->getModuleByName("tools");
            ASSERT_NE(toolsModule, nullptr);
            EXPECT_EQ(findImportedNode(result, "tools"), toolsModule);
            EXPECT_EQ(findImportedNode(result, "enabled"), toolsModule->getIdentifierByName("enabled"));
            EXPECT_EQ(findImportedNode(result, "External"), findImportedNode(result, "enabled"));
            const auto* scope = result.getScopeByAstNode(*module);
            ASSERT_NE(scope, nullptr);
            const auto* external = asRegularSymbol(scope->lookupLocal("External"));
            ASSERT_NE(external, nullptr);
            EXPECT_EQ(external->getKind(), RegularSymbolKind::VARIABLE);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, RejectsMissingAndCyclicAliasTargetsWithoutCommittingPartialBindings) {
        const std::vector<std::pair<std::string_view, std::string_view>> cases = {
            { R"({"valid":{"category":"imported","source":"extra.tools.enabled"},"broken":{"category":"imported","source":"extra.tools.absent"}})", "Could not find imported alias target" },
            { R"({"valid":{"category":"imported","source":"extra.tools.enabled"},"broken":{"category":"imported","source":"extra.tools.enabled.member"}})",
              "Could not find imported alias target" },
            { R"({"valid":{"category":"imported","source":"extra.tools.enabled"},"broken":{"category":"imported","source":"pkg.broken.broken"}})", "Cyclic imported alias" },
            { R"({"valid":{"category":"imported","source":"extra.tools.enabled"},"broken":{"category":"imported","source":"pkg.broken.link"},"link":{"category":"imported","source":"pkg.broken.broken"}})",
              "Cyclic imported alias" },
        };
        for (const auto& [contents, diagnosticPrefix] : cases) {
            SCOPED_TRACE(contents);
            writeFile("dependency_source/broken.vni", contents);
            for (const auto importSource : { "import pkg.broken.broken\n", "import pkg.broken.{valid, broken}\n", "import pkg.broken.*\n" }) {
                SCOPED_TRACE(importSource);
                std::optional<SemanticResult> result;
                ASSERT_NO_THROW(result.emplace(analyze(std::string("import pkg.api.value as kept\n") + importSource + "let valid = 0\nlet broken = 0\nexport kept\n")));

                ASSERT_TRUE(result->hasErrors());
                for (const auto& error : result->getErrors()) {
                    EXPECT_EQ(error.getPhase(), DiagnosticPhase::SEMANTIC);
                    EXPECT_TRUE(error.getMessage().starts_with(diagnosticPrefix)) << error.getMessage();
                }
                const auto* package = getImportedRootPackageByName("pkg");
                ASSERT_NE(package, nullptr);
                const auto* api = package->getModuleByName("api");
                ASSERT_NE(api, nullptr);
                EXPECT_EQ(findImportedNode(*result, "kept"), api->getIdentifierByName("value"));
                for (const auto name : { "valid", "broken", "link" }) {
                    EXPECT_EQ(findImportedNode(*result, name), nullptr);
                }
            }
        }
    }

    TEST_F(SemanticAnalyzerImportTest, ReportsInvalidImportsAsSemanticErrors) {
        for (const auto source : {
                 "import absent.api\n",
                 "import pkg.absent\n",
                 "import pkg.api.absent\n",
                 "import pkg.api.value.member\n",
                 "import pkg.api.value.{self}\n",
                 "import pkg.*\n",
                 "import pkg.{api, absent}\n",
                 "import pkg.api.{value, absent}\n",
             }) {
            SCOPED_TRACE(source);
            std::optional<SemanticResult> result;
            ASSERT_NO_THROW(result.emplace(analyze(source, "import pkg\n")));

            ASSERT_TRUE(result->hasErrors());
            for (const auto& error : result->getErrors()) {
                EXPECT_EQ(error.getPhase(), DiagnosticPhase::SEMANTIC);
            }
            for (const auto name : { "api", "value" }) {
                EXPECT_EQ(findImportedNode(*result, name), nullptr);
            }
        }
    }

    TEST_F(SemanticAnalyzerImportTest, RejectsDuplicateBindingsWithinOneImportAsSemanticErrors) {
        for (const auto source : {
                 "import pkg.api.{value, value}\n",
                 "import pkg.{api.value as same, sub.other.flag as same}\n",
                 "import pkg.api.{*, value}\n",
                 "import pkg.api.{value, *}\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_TRUE(result.hasErrors());
            for (const auto& error : result.getErrors()) {
                EXPECT_EQ(error.getPhase(), DiagnosticPhase::SEMANTIC);
                EXPECT_TRUE(error.getMessage().starts_with("Redeclaration of symbol"));
            }
            for (const auto name : { "value", "count", "same" }) {
                EXPECT_EQ(findImportedNode(result, name), nullptr);
            }
        }
    }

    TEST_F(SemanticAnalyzerImportTest, RejectsRedeclarationsAcrossImportsAndLocalDeclarations) {
        for (const auto source : {
                 "import pkg.api.value\nimport pkg.api.value\n",
                 "import pkg.api.value\nimport pkg.sub.other.flag as value\n",
                 "import pkg.api.value\nlet value = 0\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_TRUE(result.hasErrors());
            EXPECT_NE(findImportedNode(result, "value"), nullptr);
            for (const auto& error : result.getErrors()) {
                EXPECT_EQ(error.getPhase(), DiagnosticPhase::SEMANTIC);
                EXPECT_EQ(error.getMessage(), "Redeclaration of symbol 'value'");
            }
        }
    }

    TEST_F(SemanticAnalyzerImportTest, RejectsFunctionRedeclarationsAcrossImportsAndLocalDeclarations) {
        writeFile("dependency_source/api.vni", R"({"add":{"category":"func","name":"add","returnType":"void","parameters":{"value":{"category":"parameter","type":"int"}},"native":false}})");

        const auto result = analyze("import pkg.api.add\nfunc add(value: string) {}\n");

        ASSERT_EQ(result.getErrors().size(), 1);
        EXPECT_EQ(result.getErrors().front().getMessage(), "Redeclaration of symbol 'add'");
    }

    TEST_F(SemanticAnalyzerImportTest, FailedImportsPreserveEarlierBindingsWithoutDeclaringPartialNames) {
        const auto result = analyze("import pkg.api as kept\nimport pkg.sub.{other as staged, missing}\nlet staged = 0\nexport kept\n", "import pkg\n");

        ASSERT_EQ(result.getErrors().size(), 1);
        EXPECT_EQ(result.getErrors().front().getPhase(), DiagnosticPhase::SEMANTIC);
        EXPECT_EQ(result.getErrors().front().getMessage(), "Could not find imported package, module or identifier 'missing'");
        const auto* package = getImportedRootPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        EXPECT_EQ(findImportedNode(result, "kept"), package->getModuleByName("api"));
        const auto* scope = result.getScopeByAstNode(*module);
        ASSERT_NE(scope, nullptr);
        const auto* staged = asRegularSymbol(scope->lookupLocal("staged"));
        ASSERT_NE(staged, nullptr);
        EXPECT_EQ(staged->getOrigin(), RegularSymbolOrigin::LOCAL);
        EXPECT_NE(staged->getLocalNode(), nullptr);
        EXPECT_EQ(staged->getImportedNode(), nullptr);
    }

} // namespace vnlc
