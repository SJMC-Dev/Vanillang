#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/declaration/EnumDeclarationNode.hpp"
#include "ast/declaration/EnumMemberDeclarationNode.hpp"
#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include "ast/identifier/IdentifierNode.hpp"
#include "inheritance/ClassInheritanceNode.hpp"
#include "inheritance/EnumInheritanceNode.hpp"
#include "inheritance/EnumMemberInheritanceNode.hpp"
#include "inheritance/InheritanceNode.hpp"
#include "inheritance/InheritanceOrigin.hpp"
#include "inheritance/InterfaceInheritanceNode.hpp"
#include "token/Token.hpp"
#include "vni/import/ImportedClass.hpp"
#include "vni/import/ImportedEnum.hpp"
#include "vni/import/ImportedEnumMember.hpp"
#include "vni/import/ImportedInterface.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <vector>

namespace vnlc {
    namespace {
        const Token testToken(TokenKind::IDENTIFIER, "test", 1, 1, 0);
    } // namespace

    class ClassInheritanceNodeTest : public ::testing::Test {
    protected:
        const ClassDeclarationNode localClass{ false, std::make_unique<IdentifierNode>("Local", testToken, testToken), std::nullopt, {}, {}, {}, {}, {}, {}, testToken, testToken };
        const ImportedClass importedClass{ "Imported", std::nullopt, {}, false, {}, {}, {}, {} };
        const InterfaceDeclarationNode localInterface{ std::make_unique<IdentifierNode>("LocalInterface", testToken, testToken), {}, {}, testToken, testToken };
        const ImportedInterface importedInterface{ "ImportedInterface", {}, {}, {} };
    };

    TEST_F(ClassInheritanceNodeTest, LocalNodeStoresDeclarationWithoutParent) {
        const ClassInheritanceNode node(&localClass);

        EXPECT_EQ(node.getOrigin(), InheritanceOrigin::LOCAL);
        EXPECT_EQ(node.getLocalNode(), &localClass);
        EXPECT_EQ(node.getImportedNode(), nullptr);
        EXPECT_EQ(node.findParent(), nullptr);
        EXPECT_FALSE(node.hasCycle());
    }

    TEST_F(ClassInheritanceNodeTest, ImportedNodeStoresImportedClassWithoutParent) {
        const ClassInheritanceNode node(&importedClass);

        EXPECT_EQ(node.getOrigin(), InheritanceOrigin::IMPORTED);
        EXPECT_EQ(node.getLocalNode(), nullptr);
        EXPECT_EQ(node.getImportedNode(), &importedClass);
        EXPECT_EQ(node.findParent(), nullptr);
        EXPECT_FALSE(node.hasCycle());
    }

    TEST_F(ClassInheritanceNodeTest, BaseInterfaceDispatchesToClassNode) {
        ClassInheritanceNode localNode(&localClass);
        const ClassInheritanceNode importedNode(&importedClass);
        const InheritanceNode& localBase = localNode;
        const InheritanceNode& importedBase = importedNode;

        EXPECT_EQ(localBase.getOrigin(), InheritanceOrigin::LOCAL);
        EXPECT_EQ(localBase.getLocalNode(), &localClass);
        EXPECT_EQ(localBase.getImportedNode(), nullptr);
        EXPECT_EQ(importedBase.getOrigin(), InheritanceOrigin::IMPORTED);
        EXPECT_EQ(importedBase.getLocalNode(), nullptr);
        EXPECT_EQ(importedBase.getImportedNode(), &importedClass);
        EXPECT_FALSE(localBase.hasCycle());

        localNode.setParent(&localNode);

        EXPECT_TRUE(localBase.hasCycle());
    }

    TEST_F(ClassInheritanceNodeTest, ImplementedInterfacesAreKeptInOrder) {
        ClassInheritanceNode node(&localClass);
        const InterfaceInheritanceNode firstInterface(&localInterface);
        const InterfaceInheritanceNode secondInterface(&importedInterface);

        EXPECT_TRUE(node.getImplementedInterfaces().empty());

        node.addImplementedInterface(&firstInterface);
        node.addImplementedInterface(&secondInterface);

        ASSERT_EQ(node.getImplementedInterfaces().size(), 2);
        EXPECT_EQ(node.getImplementedInterfaces()[0], &firstInterface);
        EXPECT_EQ(node.getImplementedInterfaces()[1], &secondInterface);
    }

    TEST_F(ClassInheritanceNodeTest, SetParentReplacesAndClearsParent) {
        ClassInheritanceNode child(&localClass);
        const ClassInheritanceNode firstParent(&localClass);
        const ClassInheritanceNode secondParent(&importedClass);

        child.setParent(&firstParent);
        EXPECT_EQ(child.findParent(), &firstParent);

        child.setParent(&secondParent);
        EXPECT_EQ(child.findParent(), &secondParent);

        child.setParent(nullptr);
        EXPECT_EQ(child.findParent(), nullptr);
    }

    TEST_F(ClassInheritanceNodeTest, AcyclicChainAcrossOriginsHasNoCycle) {
        ClassInheritanceNode leaf(&localClass);
        ClassInheritanceNode middle(&localClass);
        ClassInheritanceNode importedBase(&importedClass);
        const ClassInheritanceNode root(&importedClass);

        leaf.setParent(&middle);
        middle.setParent(&importedBase);
        importedBase.setParent(&root);

        EXPECT_EQ(leaf.findParent(), &middle);
        EXPECT_EQ(middle.findParent(), &importedBase);
        EXPECT_EQ(importedBase.findParent(), &root);
        EXPECT_FALSE(leaf.hasCycle());
        EXPECT_FALSE(middle.hasCycle());
        EXPECT_FALSE(importedBase.hasCycle());
        EXPECT_FALSE(root.hasCycle());
    }

    TEST_F(ClassInheritanceNodeTest, SelfInheritanceIsCycle) {
        ClassInheritanceNode node(&localClass);

        node.setParent(&node);

        EXPECT_TRUE(node.hasCycle());
    }

    TEST_F(ClassInheritanceNodeTest, NodeLeadingIntoCycleReportsCycle) {
        ClassInheritanceNode outsider(&localClass);
        ClassInheritanceNode first(&localClass);
        ClassInheritanceNode second(&importedClass);

        outsider.setParent(&first);
        first.setParent(&second);
        second.setParent(&first);

        EXPECT_TRUE(outsider.hasCycle());
        EXPECT_TRUE(first.hasCycle());
        EXPECT_TRUE(second.hasCycle());
    }

    TEST_F(ClassInheritanceNodeTest, CyclesOfAnyLengthAreDetectedUntilBroken) {
        for (std::size_t tailLength = 0; tailLength <= 3; ++tailLength) {
            for (std::size_t cycleLength = 1; cycleLength <= 6; ++cycleLength) {
                SCOPED_TRACE(::testing::Message() << "tail=" << tailLength << ", cycle=" << cycleLength);

                std::vector<std::unique_ptr<ClassInheritanceNode>> nodes;
                for (std::size_t i = 0; i < tailLength + cycleLength; ++i) nodes.push_back(std::make_unique<ClassInheritanceNode>(&localClass));
                for (std::size_t i = 0; i + 1 < nodes.size(); ++i) nodes[i]->setParent(nodes[i + 1].get());
                nodes.back()->setParent(nodes[tailLength].get());

                for (const auto& node : nodes) EXPECT_TRUE(node->hasCycle());

                nodes.back()->setParent(nullptr);

                for (const auto& node : nodes) EXPECT_FALSE(node->hasCycle());
            }
        }
    }

    class InterfaceInheritanceNodeTest : public ::testing::Test {
    protected:
        const InterfaceDeclarationNode localInterface{ std::make_unique<IdentifierNode>("Local", testToken, testToken), {}, {}, testToken, testToken };
        const ImportedInterface importedInterface{ "Imported", {}, {}, {} };
    };

    TEST_F(InterfaceInheritanceNodeTest, LocalNodeStoresDeclaration) {
        const InterfaceInheritanceNode node(&localInterface);
        const InheritanceNode& base = node;

        EXPECT_EQ(node.getOrigin(), InheritanceOrigin::LOCAL);
        EXPECT_EQ(node.getLocalNode(), &localInterface);
        EXPECT_EQ(node.getImportedNode(), nullptr);
        EXPECT_EQ(base.getLocalNode(), &localInterface);
        EXPECT_EQ(base.getImportedNode(), nullptr);
        EXPECT_FALSE(base.hasCycle());
    }

    TEST_F(InterfaceInheritanceNodeTest, ImportedNodeStoresImportedInterface) {
        const InterfaceInheritanceNode node(&importedInterface);
        const InheritanceNode& base = node;

        EXPECT_EQ(node.getOrigin(), InheritanceOrigin::IMPORTED);
        EXPECT_EQ(node.getLocalNode(), nullptr);
        EXPECT_EQ(node.getImportedNode(), &importedInterface);
        EXPECT_EQ(base.getLocalNode(), nullptr);
        EXPECT_EQ(base.getImportedNode(), &importedInterface);
        EXPECT_FALSE(base.hasCycle());
    }

    class EnumInheritanceNodeTest : public ::testing::Test {
    protected:
        const EnumDeclarationNode localEnum{ std::make_unique<IdentifierNode>("Local", testToken, testToken), {}, {}, testToken, testToken };
        const ImportedEnum importedEnum{ "Imported", {}, {} };
        const EnumMemberDeclarationNode localMember{ std::make_unique<IdentifierNode>("LocalMember", testToken, testToken), {}, testToken, testToken };
        const ImportedEnumMember importedMember{ "ImportedMember", {} };
    };

    TEST_F(EnumInheritanceNodeTest, LocalEnumNodeStoresDeclaration) {
        const EnumInheritanceNode node(&localEnum);
        const InheritanceNode& base = node;

        EXPECT_EQ(node.getOrigin(), InheritanceOrigin::LOCAL);
        EXPECT_EQ(node.getLocalNode(), &localEnum);
        EXPECT_EQ(node.getImportedNode(), nullptr);
        EXPECT_EQ(base.getLocalNode(), &localEnum);
        EXPECT_EQ(base.getImportedNode(), nullptr);
        EXPECT_FALSE(base.hasCycle());
    }

    TEST_F(EnumInheritanceNodeTest, ImportedEnumNodeStoresImportedEnum) {
        const EnumInheritanceNode node(&importedEnum);
        const InheritanceNode& base = node;

        EXPECT_EQ(node.getOrigin(), InheritanceOrigin::IMPORTED);
        EXPECT_EQ(node.getLocalNode(), nullptr);
        EXPECT_EQ(node.getImportedNode(), &importedEnum);
        EXPECT_EQ(base.getLocalNode(), nullptr);
        EXPECT_EQ(base.getImportedNode(), &importedEnum);
        EXPECT_FALSE(base.hasCycle());
    }

    TEST_F(EnumInheritanceNodeTest, LocalMemberNodeStoresDeclarationWithoutParent) {
        const EnumMemberInheritanceNode node(&localMember);
        const InheritanceNode& base = node;

        EXPECT_EQ(node.getOrigin(), InheritanceOrigin::LOCAL);
        EXPECT_EQ(node.getLocalNode(), &localMember);
        EXPECT_EQ(node.getImportedNode(), nullptr);
        EXPECT_EQ(node.findParent(), nullptr);
        EXPECT_EQ(base.getLocalNode(), &localMember);
        EXPECT_EQ(base.getImportedNode(), nullptr);
        EXPECT_FALSE(base.hasCycle());
    }

    TEST_F(EnumInheritanceNodeTest, ImportedMemberNodeStoresImportedMemberWithoutParent) {
        const EnumMemberInheritanceNode node(&importedMember);
        const InheritanceNode& base = node;

        EXPECT_EQ(node.getOrigin(), InheritanceOrigin::IMPORTED);
        EXPECT_EQ(node.getLocalNode(), nullptr);
        EXPECT_EQ(node.getImportedNode(), &importedMember);
        EXPECT_EQ(node.findParent(), nullptr);
        EXPECT_EQ(base.getLocalNode(), nullptr);
        EXPECT_EQ(base.getImportedNode(), &importedMember);
        EXPECT_FALSE(base.hasCycle());
    }

    TEST_F(EnumInheritanceNodeTest, MemberSetParentReplacesAndClearsParentWithoutCycle) {
        EnumMemberInheritanceNode member(&localMember);
        const EnumInheritanceNode firstParent(&localEnum);
        const EnumInheritanceNode secondParent(&importedEnum);

        member.setParent(&firstParent);
        EXPECT_EQ(member.findParent(), &firstParent);
        EXPECT_FALSE(member.hasCycle());

        member.setParent(&secondParent);
        EXPECT_EQ(member.findParent(), &secondParent);
        EXPECT_FALSE(member.hasCycle());

        member.setParent(nullptr);
        EXPECT_EQ(member.findParent(), nullptr);
        EXPECT_FALSE(member.hasCycle());
    }
} // namespace vnlc
