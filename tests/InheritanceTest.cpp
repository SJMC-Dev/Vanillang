#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/identifier/IdentifierNode.hpp"
#include "inheritance/ClassInheritanceNode.hpp"
#include "inheritance/ClassInheritanceOrigin.hpp"
#include "token/Token.hpp"
#include "vni/import/ImportedClass.hpp"
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
    };

    TEST_F(ClassInheritanceNodeTest, LocalNodeStoresDeclarationWithoutParent) {
        const ClassInheritanceNode node(&localClass);

        EXPECT_EQ(node.getOrigin(), ClassInheritanceOrigin::LOCAL);
        EXPECT_EQ(node.getLocalNode(), &localClass);
        EXPECT_EQ(node.getImportedNode(), nullptr);
        EXPECT_EQ(node.findParent(), nullptr);
        EXPECT_FALSE(node.hasCycle());
    }

    TEST_F(ClassInheritanceNodeTest, ImportedNodeStoresImportedClassWithoutParent) {
        const ClassInheritanceNode node(&importedClass);

        EXPECT_EQ(node.getOrigin(), ClassInheritanceOrigin::IMPORTED);
        EXPECT_EQ(node.getLocalNode(), nullptr);
        EXPECT_EQ(node.getImportedNode(), &importedClass);
        EXPECT_EQ(node.findParent(), nullptr);
        EXPECT_FALSE(node.hasCycle());
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
} // namespace vnlc
