#include "ast/declaration/OperatorDeclarationKind.hpp"
#include "token/Token.hpp"
#include "type/CustomizedType.hpp"
#include "type/PrimitiveType.hpp"

#include <array>
#include <cstddef>
#include <gtest/gtest.h>
#include <initializer_list>

#include <string>
#include <string_view>
#include <vector>

namespace vnlc {
    static const Token testToken(TokenKind::IDENTIFIER, "test", 1, 1, 0);

    struct OperatorSignature {
        OperatorDeclarationKind::Kind kind;
        std::size_t parameterCount;
    };

    static std::array<const PrimitiveType*, 8> primitiveTypes() {
        return { PrimitiveType::byteType(),  PrimitiveType::shortType(),  PrimitiveType::intType(),     PrimitiveType::longType(),
                 PrimitiveType::floatType(), PrimitiveType::doubleType(), PrimitiveType::booleanType(), PrimitiveType::stringType() };
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
} // namespace vnlc
