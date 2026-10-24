#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/identifier/IdentifierNode.hpp"
#include "token/Token.hpp"
#include "type/CustomizedType.hpp"
#include "type/PrimitiveType.hpp"
#include "vni/import/ImportedClass.hpp"
#include "vni/import/ImportedInterface.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <gtest/gtest.h>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace vnlc {
    static const Token testToken(TokenKind::IDENTIFIER, "test", 1, 1, 0);

    static constexpr auto operatorNames = std::to_array<std::string_view>({ "addition",
                                                                            "subtraction",
                                                                            "multiplication",
                                                                            "division",
                                                                            "integerDivision",
                                                                            "modulo",
                                                                            "exponent",
                                                                            "equality",
                                                                            "inequality",
                                                                            "lessThan",
                                                                            "greaterThan",
                                                                            "lessThanOrEqual",
                                                                            "greaterThanOrEqual",
                                                                            "bitwiseAnd",
                                                                            "bitwiseOr",
                                                                            "bitwiseXor",
                                                                            "bitwiseNot",
                                                                            "shiftLeft",
                                                                            "shiftRight",
                                                                            "unsignedShiftRight",
                                                                            "call",
                                                                            "subscript",
                                                                            "range",
                                                                            "assignment",
                                                                            "additionAssignment",
                                                                            "subtractionAssignment",
                                                                            "multiplicationAssignment",
                                                                            "divisionAssignment",
                                                                            "integerDivisionAssignment",
                                                                            "moduloAssignment",
                                                                            "exponentAssignment",
                                                                            "bitwiseAndAssignment",
                                                                            "bitwiseOrAssignment",
                                                                            "bitwiseXorAssignment",
                                                                            "shiftLeftAssignment",
                                                                            "shiftRightAssignment",
                                                                            "unsignedShiftRightAssignment",
                                                                            "nullishCoalescingAssignment",
                                                                            "nullishCoalescing",
                                                                            "referenceEquality",
                                                                            "referenceInequality",
                                                                            "logicalAnd",
                                                                            "logicalOr",
                                                                            "logicalNot",
                                                                            "instanceOf",
                                                                            "unaryPlus",
                                                                            "unaryMinus" });

    struct OperatorSignature {
        OperatorDeclarationKind::Kind kind;
        std::size_t parameterCount;
    };

    static std::array<const PrimitiveType*, 8> primitiveTypes() {
        return { PrimitiveType::byteType(),  PrimitiveType::shortType(),  PrimitiveType::intType(),     PrimitiveType::longType(),
                 PrimitiveType::floatType(), PrimitiveType::doubleType(), PrimitiveType::booleanType(), PrimitiveType::stringType() };
    }

    static void expectOnlyOperators(const Type& type, std::initializer_list<std::string_view> supportedOperators) {
        for (const auto name : operatorNames) {
            SCOPED_TRACE(::testing::Message() << type.getFullTypeName() << " operator " << name);
            EXPECT_EQ(type.canUseOperator(name), std::ranges::find(supportedOperators, name) != supportedOperators.end());
        }
    }

    static std::vector<std::unique_ptr<OperatorDeclarationNode>> makeOperators(std::initializer_list<OperatorSignature> signatures, OperatorDeclarationKind::Context context) {
        std::vector<std::unique_ptr<OperatorDeclarationNode>> operators;
        for (const auto& signature : signatures) {
            std::vector<std::unique_ptr<ValueDeclarationNode>> parameters;
            for (std::size_t index = 0; index < signature.parameterCount; ++index) {
                parameters.push_back(
                    std::make_unique<ValueDeclarationNode>(
                        ValueDeclarationKind::Kind::PARAMETER,
                        ValueDeclarationKind::Context::FUNCTION,
                        ValueDeclarationKind::AccessModifier::PUBLIC,
                        std::make_unique<IdentifierNode>("other", testToken, testToken),
                        std::nullopt,
                        std::nullopt,
                        testToken,
                        testToken
                    )
                );
            }

            operators.push_back(
                std::make_unique<OperatorDeclarationNode>(
                    signature.kind,
                    OperatorDeclarationKind::Implementation::NATIVE,
                    context,
                    OperatorDeclarationKind::AccessModifier::PUBLIC,
                    std::move(parameters),
                    std::nullopt,
                    std::nullopt,
                    testToken,
                    testToken
                )
            );
        }
        return operators;
    }

    static std::unordered_map<std::string, std::unique_ptr<ImportedOperator>> makeImportedOperators(std::initializer_list<std::string_view> internalNames) {
        std::unordered_map<std::string, std::unique_ptr<ImportedOperator>> operators;
        for (const auto internalName : internalNames) {
            operators.emplace(
                std::string(internalName),
                std::make_unique<ImportedOperator>(internalName, "pkg.Value", std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>{}, true, "public")
            );
        }
        return operators;
    }

    static std::unique_ptr<ClassDeclarationNode> makeLocalClass(std::vector<std::unique_ptr<OperatorDeclarationNode>>&& operators) {
        return std::make_unique<ClassDeclarationNode>(
            false,
            std::make_unique<IdentifierNode>("Value", testToken, testToken),
            std::nullopt,
            std::vector<std::unique_ptr<TypeReferenceNode>>{},
            std::vector<std::unique_ptr<IdentifierNode>>{},
            std::vector<std::unique_ptr<ValueDeclarationNode>>{},
            std::vector<std::unique_ptr<FunctionDeclarationNode>>{},
            std::vector<std::unique_ptr<ConstructorDeclarationNode>>{},
            std::move(operators),
            testToken,
            testToken
        );
    }

    TEST(TypeConversionTest, PrimitiveConversionMatrix) {
        const auto types = primitiveTypes();
        constexpr bool expected[8][8] = { { true, true, true, true, false, false, true, false },   { false, true, true, true, false, false, true, false },
                                          { false, false, true, true, false, false, true, false }, { false, false, false, true, false, false, true, false },
                                          { false, false, false, false, true, true, true, false }, { false, false, false, false, false, true, true, false },
                                          { true, true, true, true, true, true, true, false },     { false, false, false, false, false, false, false, true } };

        for (std::size_t source = 0; source < types.size(); ++source) {
            for (std::size_t target = 0; target < types.size(); ++target) {
                SCOPED_TRACE(::testing::Message() << types[source]->getFullTypeName() << " -> " << types[target]->getFullTypeName());
                EXPECT_EQ(types[source]->canConvertTo(*types[target]), expected[source][target]);
            }
        }
    }

    TEST(TypeConversionTest, CustomizedTypesConvertOnlyToThemselvesUntilConversionMechanismsExist) {
        constexpr std::array kinds = { CustomizedTypeKind::CLASS,
                                       CustomizedTypeKind::INTERFACE,
                                       CustomizedTypeKind::ENUM,
                                       CustomizedTypeKind::ENUM_MEMBER,
                                       CustomizedTypeKind::GENERIC_PARAMETER };

        for (const auto kind : kinds) {
            const CustomizedType localType(kind, "pkg.Value", {}, static_cast<const TypeDeclarationNode*>(nullptr));
            const CustomizedType importedType(kind, "pkg.Value", {}, static_cast<const ImportedIdentifier*>(nullptr));
            const std::array<const CustomizedType*, 2> types = { &localType, &importedType };

            for (const auto* type : types) {
                SCOPED_TRACE(::testing::Message() << "kind=" << static_cast<int>(kind) << ", origin=" << static_cast<int>(type->getOrigin()));
                EXPECT_TRUE(type->canConvertTo(*type));

                for (const auto* primitive : primitiveTypes()) {
                    SCOPED_TRACE(primitive->getFullTypeName());
                    EXPECT_FALSE(type->canConvertTo(*primitive));
                    EXPECT_FALSE(primitive->canConvertTo(*type));
                }
            }
        }
    }

    TEST(TypeConversionTest, MatchingCustomizedTypeNamesConvertAcrossInstancesAndOrigins) {
        const CustomizedType source(CustomizedTypeKind::CLASS, "pkg.Value", {}, static_cast<const TypeDeclarationNode*>(nullptr));
        const CustomizedType localTarget(CustomizedTypeKind::CLASS, "pkg.Value", {}, static_cast<const TypeDeclarationNode*>(nullptr));
        const CustomizedType importedTarget(CustomizedTypeKind::CLASS, "pkg.Value", {}, static_cast<const ImportedIdentifier*>(nullptr));

        EXPECT_TRUE(source.canConvertTo(localTarget));
        EXPECT_TRUE(localTarget.canConvertTo(source));
        EXPECT_TRUE(source.canConvertTo(importedTarget));
        EXPECT_TRUE(importedTarget.canConvertTo(source));
    }

    TEST(TypeConversionTest, DistinctCustomizedTypesDoNotConvertWithoutAnInheritanceGraph) {
        const CustomizedType source(CustomizedTypeKind::CLASS, "pkg.Value", {}, static_cast<const TypeDeclarationNode*>(nullptr));
        const CustomizedType otherClass(CustomizedTypeKind::CLASS, "other.Value", {}, static_cast<const TypeDeclarationNode*>(nullptr));
        const CustomizedType otherInterface(CustomizedTypeKind::INTERFACE, "pkg.Interface", {}, static_cast<const ImportedIdentifier*>(nullptr));

        EXPECT_FALSE(source.canConvertTo(otherClass));
        EXPECT_FALSE(otherClass.canConvertTo(source));
        EXPECT_FALSE(source.canConvertTo(otherInterface));
        EXPECT_FALSE(otherInterface.canConvertTo(source));
    }

    TEST(TypeConversionTest, GenericCustomizedTypesCompareTheirCompleteNames) {
        const CustomizedType source(CustomizedTypeKind::CLASS, "pkg.Box<int>", { PrimitiveType::intType() }, static_cast<const TypeDeclarationNode*>(nullptr));
        const CustomizedType sameType(CustomizedTypeKind::CLASS, "pkg.Box<int>", { PrimitiveType::intType() }, static_cast<const ImportedIdentifier*>(nullptr));
        const CustomizedType widerArgument(CustomizedTypeKind::CLASS, "pkg.Box<long>", { PrimitiveType::longType() }, static_cast<const TypeDeclarationNode*>(nullptr));

        EXPECT_TRUE(source.canConvertTo(sameType));
        EXPECT_TRUE(sameType.canConvertTo(source));
        EXPECT_FALSE(source.canConvertTo(widerArgument));
        EXPECT_FALSE(widerArgument.canConvertTo(source));
    }

    TEST(TypeOperatorTest, PrimitiveOperatorMatrix) {
        struct OperatorExpectation {
            std::string_view name;
            std::array<bool, 8> supported;
        };

        constexpr auto expectations = std::to_array<OperatorExpectation>({
            { "addition", { true, true, true, true, true, true, false, true } },
            { "subtraction", { true, true, true, true, true, true, false, false } },
            { "multiplication", { true, true, true, true, true, true, false, false } },
            { "division", { true, true, true, true, true, true, false, false } },
            { "integerDivision", { true, true, true, true, true, true, false, false } },
            { "modulo", { true, true, true, true, true, true, false, false } },
            { "exponent", { true, true, true, true, true, true, false, false } },
            { "equality", { true, true, true, true, true, true, true, true } },
            { "inequality", { true, true, true, true, true, true, true, true } },
            { "lessThan", { true, true, true, true, true, true, false, false } },
            { "greaterThan", { true, true, true, true, true, true, false, false } },
            { "lessThanOrEqual", { true, true, true, true, true, true, false, false } },
            { "greaterThanOrEqual", { true, true, true, true, true, true, false, false } },
            { "bitwiseAnd", { true, true, true, true, false, false, true, false } },
            { "bitwiseOr", { true, true, true, true, false, false, true, false } },
            { "bitwiseXor", { true, true, true, true, false, false, true, false } },
            { "bitwiseNot", { true, true, true, true, false, false, false, false } },
            { "shiftLeft", { true, true, true, true, false, false, false, false } },
            { "shiftRight", { true, true, true, true, false, false, false, false } },
            { "unsignedShiftRight", { true, true, true, true, false, false, false, false } },
            { "call", { false, false, false, false, false, false, false, false } },
            { "subscript", { false, false, false, false, false, false, false, true } },
            { "range", { true, true, true, true, false, false, false, false } },
            { "assignment", { true, true, true, true, true, true, true, true } },
            { "additionAssignment", { true, true, true, true, true, true, false, true } },
            { "subtractionAssignment", { true, true, true, true, true, true, false, false } },
            { "multiplicationAssignment", { true, true, true, true, true, true, false, false } },
            { "divisionAssignment", { true, true, true, true, true, true, false, false } },
            { "integerDivisionAssignment", { true, true, true, true, true, true, false, false } },
            { "moduloAssignment", { true, true, true, true, true, true, false, false } },
            { "exponentAssignment", { true, true, true, true, true, true, false, false } },
            { "bitwiseAndAssignment", { true, true, true, true, false, false, true, false } },
            { "bitwiseOrAssignment", { true, true, true, true, false, false, true, false } },
            { "bitwiseXorAssignment", { true, true, true, true, false, false, true, false } },
            { "shiftLeftAssignment", { true, true, true, true, false, false, false, false } },
            { "shiftRightAssignment", { true, true, true, true, false, false, false, false } },
            { "unsignedShiftRightAssignment", { true, true, true, true, false, false, false, false } },
            { "nullishCoalescingAssignment", { false, false, false, false, false, false, false, false } },
            { "nullishCoalescing", { false, false, false, false, false, false, false, false } },
            { "referenceEquality", { false, false, false, false, false, false, false, false } },
            { "referenceInequality", { false, false, false, false, false, false, false, false } },
            { "logicalAnd", { false, false, false, false, false, false, true, false } },
            { "logicalOr", { false, false, false, false, false, false, true, false } },
            { "logicalNot", { false, false, false, false, false, false, true, false } },
            { "instanceOf", { false, false, false, false, false, false, false, false } },
            { "unaryPlus", { true, true, true, true, true, true, false, false } },
            { "unaryMinus", { true, true, true, true, true, true, false, false } },
        });
        static_assert(expectations.size() == operatorNames.size());

        const auto types = primitiveTypes();
        for (const auto& expectation : expectations) {
            for (std::size_t index = 0; index < types.size(); ++index) {
                SCOPED_TRACE(::testing::Message() << types[index]->getFullTypeName() << " operator " << expectation.name);
                EXPECT_EQ(types[index]->canUseOperator(expectation.name), expectation.supported[index]);
            }
        }
    }

    TEST(TypeOperatorTest, PrimitiveTypesRejectUnknownOperatorNames) {
        constexpr auto unknownNames = std::to_array<std::string_view>(
            { "", "+", "Addition", "Assignment", "assignmentAssignment", "equalityAssignment", "logicalAndAssignment", "unaryMinusAssignment", "vnl-operator-addition()" }
        );

        for (const auto* type : primitiveTypes()) {
            for (const auto name : unknownNames) {
                SCOPED_TRACE(::testing::Message() << type->getFullTypeName() << " operator '" << name << "'");
                EXPECT_FALSE(type->canUseOperator(name));
            }
        }
    }

    TEST(TypeOperatorTest, LocalClassSupportsDeclaredOverloadsAndTheirCompoundAssignments) {
        const auto classNode = makeLocalClass(makeOperators(
            { { OperatorDeclarationKind::Kind::ADDITION, 1 },
              { OperatorDeclarationKind::Kind::ADDITION, 1 },
              { OperatorDeclarationKind::Kind::EQUAL, 1 },
              { OperatorDeclarationKind::Kind::SHIFT_RIGHT_UNSIGNED, 1 },
              { OperatorDeclarationKind::Kind::SUBSCRIPT, 1 } },
            OperatorDeclarationKind::Context::CLASS
        ));
        const CustomizedType type(CustomizedTypeKind::CLASS, "pkg.Value", {}, classNode.get());

        expectOnlyOperators(
            type,
            { "addition",
              "equality",
              "unsignedShiftRight",
              "subscript",
              "assignment",
              "additionAssignment",
              "unsignedShiftRightAssignment",
              "referenceEquality",
              "referenceInequality",
              "instanceOf" }
        );
    }

    TEST(TypeOperatorTest, LocalClassDistinguishesUnaryAndBinaryPlusMinusOverloads) {
        const auto classNode = makeLocalClass(makeOperators(
            { { OperatorDeclarationKind::Kind::SUBTRACTION, 0 }, { OperatorDeclarationKind::Kind::ADDITION, 1 }, { OperatorDeclarationKind::Kind::BITWISE_NOT, 0 } },
            OperatorDeclarationKind::Context::CLASS
        ));
        const CustomizedType type(CustomizedTypeKind::CLASS, "pkg.Value", {}, classNode.get());

        expectOnlyOperators(type, { "unaryMinus", "addition", "additionAssignment", "bitwiseNot", "assignment", "referenceEquality", "referenceInequality", "instanceOf" });
    }

    TEST(TypeOperatorTest, LocalInterfaceSupportsDeclaredOperatorSignatures) {
        const InterfaceDeclarationNode interfaceNode(
            std::make_unique<IdentifierNode>("Value", testToken, testToken),
            {},
            {},
            makeOperators(
                { { OperatorDeclarationKind::Kind::LESS_THAN_OR_EQUAL, 1 }, { OperatorDeclarationKind::Kind::CALL, 0 }, { OperatorDeclarationKind::Kind::ADDITION, 0 } },
                OperatorDeclarationKind::Context::INTERFACE
            ),
            testToken,
            testToken
        );
        const CustomizedType type(CustomizedTypeKind::INTERFACE, "pkg.Value", {}, &interfaceNode);

        expectOnlyOperators(type, { "lessThanOrEqual", "call", "unaryPlus", "assignment", "referenceEquality", "referenceInequality", "instanceOf" });
    }

    TEST(TypeOperatorTest, ImportedClassMatchesOperatorNamesInInternalNamesExactly) {
        const ImportedClass classNode(
            "Value",
            std::nullopt,
            {},
            false,
            {},
            {},
            {},
            {},
            makeImportedOperators(
                { "vnl-operator-addition(pkg.Value)",
                  "vnl-operator-addition(int)",
                  "vnl-operator-unsignedShiftRight(int)",
                  "vnl-operator-lessThanOrEqual(pkg.Value)",
                  "vnl-operator-subtraction()" }
            )
        );
        const CustomizedType type(CustomizedTypeKind::CLASS, "pkg.Value", {}, &classNode);

        expectOnlyOperators(
            type,
            { "addition",
              "additionAssignment",
              "unsignedShiftRight",
              "unsignedShiftRightAssignment",
              "lessThanOrEqual",
              "unaryMinus",
              "assignment",
              "referenceEquality",
              "referenceInequality",
              "instanceOf" }
        );
    }

    TEST(TypeOperatorTest, ImportedInterfaceSupportsDeclaredOperatorSignatures) {
        const ImportedInterface interfaceNode("Value", {}, {}, makeImportedOperators({ "vnl-operator-subscript(int)", "vnl-operator-range(int)", "vnl-operator-addition()" }));
        const CustomizedType type(CustomizedTypeKind::INTERFACE, "pkg.Value", {}, &interfaceNode);

        expectOnlyOperators(type, { "subscript", "range", "unaryPlus", "assignment", "referenceEquality", "referenceInequality", "instanceOf" });
    }

    TEST(TypeOperatorTest, ImportedOperatorsWithUnexpectedInternalNamesAreIgnored) {
        const ImportedClass classNode(
            "Value",
            std::nullopt,
            {},
            false,
            {},
            {},
            {},
            {},
            makeImportedOperators({ "addition", "vnl-method-addition(int)", "vnl-operator-", "vnl-operator-logicalAnd(pkg.Value)", "vnl-operator-logicalNot()" })
        );
        const CustomizedType type(CustomizedTypeKind::CLASS, "pkg.Value", {}, &classNode);

        expectOnlyOperators(type, { "assignment", "referenceEquality", "referenceInequality", "instanceOf" });
        EXPECT_FALSE(type.canUseOperator(""));
    }

    TEST(TypeOperatorTest, ReferenceTypesWithoutOverloadsSupportOnlyAssignmentAndReferenceOperators) {
        constexpr std::array kinds = { CustomizedTypeKind::CLASS, CustomizedTypeKind::INTERFACE, CustomizedTypeKind::ENUM, CustomizedTypeKind::ENUM_MEMBER };
        const auto emptyClassNode = makeLocalClass({});
        const ImportedClass emptyImportedClass("Value", std::nullopt, {}, false, {}, {}, {}, {});

        for (const auto kind : kinds) {
            const CustomizedType localType(kind, "pkg.Value", {}, static_cast<const TypeDeclarationNode*>(nullptr));
            const CustomizedType importedType(kind, "pkg.Value", {}, static_cast<const ImportedIdentifier*>(nullptr));

            SCOPED_TRACE(::testing::Message() << "kind=" << static_cast<int>(kind));
            expectOnlyOperators(localType, { "assignment", "referenceEquality", "referenceInequality", "instanceOf" });
            expectOnlyOperators(importedType, { "assignment", "referenceEquality", "referenceInequality", "instanceOf" });
        }

        expectOnlyOperators(CustomizedType(CustomizedTypeKind::CLASS, "pkg.Value", {}, emptyClassNode.get()), { "assignment", "referenceEquality", "referenceInequality", "instanceOf" });
        expectOnlyOperators(CustomizedType(CustomizedTypeKind::CLASS, "pkg.Value", {}, &emptyImportedClass), { "assignment", "referenceEquality", "referenceInequality", "instanceOf" });
    }

    TEST(TypeOperatorTest, GenericParametersDeferAllOperatorChecksToRuntime) {
        const CustomizedType localType(CustomizedTypeKind::GENERIC_PARAMETER, "T", {}, static_cast<const TypeDeclarationNode*>(nullptr));
        const CustomizedType importedType(CustomizedTypeKind::GENERIC_PARAMETER, "T", {}, static_cast<const ImportedIdentifier*>(nullptr));
        const CustomizedType optionalNamedType(CustomizedTypeKind::GENERIC_PARAMETER, "vanillang.typesystem.Optional<T>", {}, static_cast<const TypeDeclarationNode*>(nullptr));
        const std::array<const CustomizedType*, 3> types = { &localType, &importedType, &optionalNamedType };

        for (const auto* type : types) {
            for (const auto name : operatorNames) {
                SCOPED_TRACE(::testing::Message() << type->getFullTypeName() << " operator " << name);
                EXPECT_TRUE(type->canUseOperator(name));
            }
        }
    }

    TEST(TypeOperatorTest, OnlyOptionalTypesSupportNullishCoalescingOperators) {
        const CustomizedType optionalPrimitive(CustomizedTypeKind::ENUM, "vanillang.typesystem.Optional<int>", { PrimitiveType::intType() }, static_cast<const TypeDeclarationNode*>(nullptr));
        const CustomizedType optionalCustomized(CustomizedTypeKind::ENUM, "vanillang.typesystem.Optional<pkg.Value>", {}, static_cast<const ImportedIdentifier*>(nullptr));
        const CustomizedType otherOptional(CustomizedTypeKind::ENUM, "pkg.Optional<int>", { PrimitiveType::intType() }, static_cast<const TypeDeclarationNode*>(nullptr));
        const CustomizedType optionalLookalike(CustomizedTypeKind::ENUM, "vanillang.typesystem.OptionalValue", {}, static_cast<const TypeDeclarationNode*>(nullptr));

        expectOnlyOperators(optionalPrimitive, { "assignment", "nullishCoalescing", "nullishCoalescingAssignment", "referenceEquality", "referenceInequality", "instanceOf" });
        expectOnlyOperators(optionalCustomized, { "assignment", "nullishCoalescing", "nullishCoalescingAssignment", "referenceEquality", "referenceInequality", "instanceOf" });
        expectOnlyOperators(otherOptional, { "assignment", "referenceEquality", "referenceInequality", "instanceOf" });
        expectOnlyOperators(optionalLookalike, { "assignment", "referenceEquality", "referenceInequality", "instanceOf" });
    }
} // namespace vnlc
