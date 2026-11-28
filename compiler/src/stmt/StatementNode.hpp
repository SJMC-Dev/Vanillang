#pragma once

#include "SwitchItem.hpp"
#include "ast/AstNode.hpp"
#include "decl/DeclarationNode.hpp"
#include "expr/ExpressionNode.hpp"
#include "identifier/IdentifierNode.hpp"
#include "metadata/MetadataTerm.hpp"
#include "prop/LanguageProperty.hpp"
#include <memory>
#include <variant>
#include <vector>

namespace vnlc {
    class ValueDeclarationNode;

    class StatementNode : public AstNode {
    protected:
        StatementNode(const Token& firstToken, const Token& lastToken);

    public:
        StatementNode() = delete;
    };

    class BlockStatementNode : public StatementNode {
    private:
        std::vector<std::unique_ptr<StatementNode>> statements;

        bool includeMetadata;
        std::vector<MetadataTerm> metadataTerms;

    public:
        BlockStatementNode() = delete;

        BlockStatementNode(std::vector<std::unique_ptr<StatementNode>>&& statements, const Token& firstToken, const Token& lastToken) noexcept;

        BlockStatementNode(std::vector<std::unique_ptr<StatementNode>>&& statements, const Token& firstToken, const Token& lastToken, std::vector<MetadataTerm>&& metadataTerms) noexcept;

        [[nodiscard]] const std::vector<std::unique_ptr<StatementNode>>& getStatements() const noexcept;
        [[nodiscard]] bool doesIncludeMetadata() const noexcept;
        [[nodiscard]] const std::vector<MetadataTerm>& getMetadataTerms() const noexcept;
    };

    class AssignmentStatementNode : public StatementNode {
    private:
        AssignmentKind kind;

        std::unique_ptr<ExpressionNode> left;
        std::unique_ptr<ExpressionNode> right;

        bool includeTryKeyword;

    public:
        AssignmentStatementNode() = delete;

        AssignmentStatementNode(
            AssignmentKind kind,
            std::unique_ptr<ExpressionNode>&& left,
            std::unique_ptr<ExpressionNode>&& right,
            bool includeTryKeyword,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] AssignmentKind getKind() const noexcept;
        [[nodiscard]] const ExpressionNode& getLeft() const noexcept;
        [[nodiscard]] const ExpressionNode& getRight() const noexcept;
        [[nodiscard]] bool doesIncludeTryKeyword() const noexcept;
    };

    class ExpressionStatementNode : public StatementNode {
    private:
        std::unique_ptr<ExpressionNode> expression;

        bool includeTryKeyword;

    public:
        ExpressionStatementNode() = delete;

        ExpressionStatementNode(std::unique_ptr<ExpressionNode>&& expression, bool includeTryKeyword, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ExpressionNode& getExpression() const noexcept;
        [[nodiscard]] bool doesIncludeTryKeyword() const noexcept;
    };

    class ValueDeclarationStatementNode : public StatementNode {
    private:
        std::unique_ptr<ValueDeclarationNode> declaration;

    public:
        ValueDeclarationStatementNode() = delete;

        ValueDeclarationStatementNode(std::unique_ptr<ValueDeclarationNode>&& declaration, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ValueDeclarationNode& getDeclaration() const noexcept;
    };

    class ControlFlowStatementNode : public StatementNode {
    protected:
        ControlFlowStatementNode(const Token& firstToken, const Token& lastToken) noexcept;

    public:
        ControlFlowStatementNode() = delete;
    };

    class IfStatementNode : public ControlFlowStatementNode {
    private:
        using Condition = std::variant<std::unique_ptr<ExpressionNode>, std::unique_ptr<ValueDeclarationNode>>;

        Condition condition;
        std::unique_ptr<StatementNode> thenBranch;
        std::unique_ptr<StatementNode> elseBranch; // nullptr if no else branch

    public:
        IfStatementNode() = delete;

        IfStatementNode(
            std::unique_ptr<ExpressionNode>&& conditionExpression,
            std::unique_ptr<StatementNode>&& thenBranch,
            std::unique_ptr<StatementNode>&& elseBranch,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        IfStatementNode(
            std::unique_ptr<ValueDeclarationNode>&& conditionDeclaration,
            std::unique_ptr<StatementNode>&& thenBranch,
            std::unique_ptr<StatementNode>&& elseBranch,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const Condition& getCondition() const noexcept;
        [[nodiscard]] const StatementNode& getThenBranch() const noexcept;
        [[nodiscard]] const StatementNode* getElseBranch() const noexcept;
    };

    class LoopStatementNode : public ControlFlowStatementNode {
    private:
        std::unique_ptr<IdentifierNode> label; // nullptr if no label

    protected:
        LoopStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept;

    public:
        LoopStatementNode() = delete;

        [[nodiscard]] const IdentifierNode* getLabel() const noexcept;
    };

    class ForStatementNode : public LoopStatementNode {
    private:
        std::unique_ptr<ValueDeclarationNode> loopVariable;
        std::unique_ptr<ExpressionNode> iterableExpression;
        std::unique_ptr<StatementNode> body;

    public:
        ForStatementNode() = delete;

        ForStatementNode(
            std::unique_ptr<ValueDeclarationNode>&& loopVariable,
            std::unique_ptr<ExpressionNode>&& iterableExpression,
            std::unique_ptr<StatementNode>&& body,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        ForStatementNode(
            std::unique_ptr<IdentifierNode>&& label,
            std::unique_ptr<ValueDeclarationNode>&& loopVariable,
            std::unique_ptr<ExpressionNode>&& iterableExpression,
            std::unique_ptr<StatementNode>&& body,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const ValueDeclarationNode& getLoopVariable() const noexcept;
        [[nodiscard]] const ExpressionNode& getIterableExpression() const noexcept;
        [[nodiscard]] const StatementNode& getBody() const noexcept;
    };

    class WhileStatementNode : public LoopStatementNode {
    private:
        using Condition = std::variant<std::unique_ptr<ExpressionNode>, std::unique_ptr<ValueDeclarationNode>>;

        Condition condition;
        std::unique_ptr<StatementNode> body;

    public:
        WhileStatementNode() = delete;

        WhileStatementNode(std::unique_ptr<ExpressionNode>&& conditionExpression, std::unique_ptr<StatementNode>&& body, const Token& firstToken, const Token& lastToken) noexcept;

        WhileStatementNode(std::unique_ptr<ValueDeclarationNode>&& conditionDeclaration, std::unique_ptr<StatementNode>&& body, const Token& firstToken, const Token& lastToken) noexcept;

        WhileStatementNode(
            std::unique_ptr<IdentifierNode>&& label,
            std::unique_ptr<ExpressionNode>&& conditionExpression,
            std::unique_ptr<StatementNode>&& body,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        WhileStatementNode(
            std::unique_ptr<IdentifierNode>&& label,
            std::unique_ptr<ValueDeclarationNode>&& conditionDeclaration,
            std::unique_ptr<StatementNode>&& body,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const Condition& getCondition() const noexcept;
        [[nodiscard]] const StatementNode& getBody() const noexcept;
    };

    class SwitchStatementNode : public ControlFlowStatementNode {
    private:
        using Condition = std::variant<std::unique_ptr<ExpressionNode>, std::unique_ptr<ValueDeclarationNode>>;
        using Items = std::variant<std::vector<LiteralMatchItem>, std::vector<TypeMatchItem>>;

        Condition condition;
        Items items;
        std::unique_ptr<StatementNode> defaultCaseBody; // nullptr if no default case

    public:
        SwitchStatementNode() = delete;

        SwitchStatementNode(
            std::unique_ptr<ExpressionNode>&& conditionExpression,
            std::vector<LiteralMatchItem>&& literalMatchItems,
            std::unique_ptr<StatementNode>&& defaultCaseBody,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        SwitchStatementNode(
            std::unique_ptr<ValueDeclarationNode>&& conditionDeclaration,
            std::vector<LiteralMatchItem>&& literalMatchItems,
            std::unique_ptr<StatementNode>&& defaultCaseBody,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        SwitchStatementNode(
            std::unique_ptr<ExpressionNode>&& conditionExpression,
            std::vector<TypeMatchItem>&& typeMatchItems,
            std::unique_ptr<StatementNode>&& defaultCaseBody,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        SwitchStatementNode(
            std::unique_ptr<ValueDeclarationNode>&& conditionDeclaration,
            std::vector<TypeMatchItem>&& typeMatchItems,
            std::unique_ptr<StatementNode>&& defaultCaseBody,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const Condition& getCondition() const noexcept;
        [[nodiscard]] const Items& getItems() const noexcept;
        [[nodiscard]] const StatementNode* getDefaultCaseBody() const noexcept;
    };

    class ReturnStatementNode : public ControlFlowStatementNode {
    private:
        std::unique_ptr<ExpressionNode> returnValue; // nullptr if no return value

    public:
        ReturnStatementNode() = delete;

        ReturnStatementNode(const Token& firstToken, const Token& lastToken) noexcept;
        ReturnStatementNode(std::unique_ptr<ExpressionNode>&& returnValue, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ExpressionNode* getReturnValue() const noexcept;
    };

    class BreakStatementNode : public ControlFlowStatementNode {
    private:
        std::unique_ptr<IdentifierNode> label; // nullptr if no label

    public:
        BreakStatementNode() = delete;

        BreakStatementNode(const Token& firstToken, const Token& lastToken) noexcept;
        BreakStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const IdentifierNode* getLabel() const noexcept;
    };

    class ContinueStatementNode : public ControlFlowStatementNode {
    private:
        std::unique_ptr<IdentifierNode> label; // nullptr if no label

    public:
        ContinueStatementNode() = delete;

        ContinueStatementNode(const Token& firstToken, const Token& lastToken) noexcept;
        ContinueStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const IdentifierNode* getLabel() const noexcept;
    };

    class ReloadStatementNode : public ControlFlowStatementNode {
    public:
        ReloadStatementNode() = delete;

        ReloadStatementNode(const Token& firstToken, const Token& lastToken) noexcept;
    };
} // namespace vnlc
