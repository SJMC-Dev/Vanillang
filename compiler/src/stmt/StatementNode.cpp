#include "StatementNode.hpp"
#include <memory>
#include <utility>

namespace vnlc {
    StatementNode::StatementNode(const Token& firstToken, const Token& lastToken) : AstNode(firstToken, lastToken) {}

    BlockStatementNode::BlockStatementNode(std::vector<std::unique_ptr<StatementNode>>&& statements, const Token& firstToken, const Token& lastToken) noexcept
        : StatementNode(firstToken, lastToken),
          statements(std::move(statements)),
          includeMetadata(false),
          metadataTerms() {}

    BlockStatementNode::BlockStatementNode(
        std::vector<std::unique_ptr<StatementNode>>&& statements,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<MetadataTerm>&& metadataTerms
    ) noexcept
        : StatementNode(firstToken, lastToken),
          statements(std::move(statements)),
          includeMetadata(true),
          metadataTerms(std::move(metadataTerms)) {}

    const std::vector<std::unique_ptr<StatementNode>>& BlockStatementNode::getStatements() const noexcept {
        return statements;
    }

    bool BlockStatementNode::doesIncludeMetadata() const noexcept {
        return includeMetadata;
    }

    const std::vector<MetadataTerm>& BlockStatementNode::getMetadataTerms() const noexcept {
        return metadataTerms;
    }

    AssignmentStatementNode::AssignmentStatementNode(
        AssignmentKind kind,
        std::unique_ptr<ExpressionNode>&& left,
        std::unique_ptr<ExpressionNode>&& right,
        bool includeTryKeyword,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : StatementNode(firstToken, lastToken),
          kind(kind),
          left(std::move(left)),
          right(std::move(right)),
          includeTryKeyword(includeTryKeyword) {}

    AssignmentKind AssignmentStatementNode::getKind() const noexcept {
        return kind;
    }

    const ExpressionNode& AssignmentStatementNode::getLeft() const noexcept {
        return *left;
    }

    const ExpressionNode& AssignmentStatementNode::getRight() const noexcept {
        return *right;
    }

    bool AssignmentStatementNode::doesIncludeTryKeyword() const noexcept {
        return includeTryKeyword;
    }

    ExpressionStatementNode::ExpressionStatementNode(std::unique_ptr<ExpressionNode>&& expression, bool includeTryKeyword, const Token& firstToken, const Token& lastToken) noexcept
        : StatementNode(firstToken, lastToken),
          expression(std::move(expression)),
          includeTryKeyword(includeTryKeyword) {}

    const ExpressionNode& ExpressionStatementNode::getExpression() const noexcept {
        return *expression;
    }

    bool ExpressionStatementNode::doesIncludeTryKeyword() const noexcept {
        return includeTryKeyword;
    }

    ValueDeclarationStatementNode::ValueDeclarationStatementNode(std::unique_ptr<ValueDeclarationNode>&& declaration, const Token& firstToken, const Token& lastToken) noexcept
        : StatementNode(firstToken, lastToken),
          declaration(std::move(declaration)) {}

    const ValueDeclarationNode& ValueDeclarationStatementNode::getDeclaration() const noexcept {
        return *declaration;
    }

    ControlFlowStatementNode::ControlFlowStatementNode(const Token& firstToken, const Token& lastToken) noexcept : StatementNode(firstToken, lastToken) {}

    IfStatementNode::IfStatementNode(
        std::unique_ptr<ExpressionNode>&& conditionExpression,
        std::unique_ptr<StatementNode>&& thenBranch,
        std::unique_ptr<StatementNode>&& elseBranch,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          condition(std::move(conditionExpression)),
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}

    IfStatementNode::IfStatementNode(
        std::unique_ptr<ValueDeclarationNode>&& conditionDeclaration,
        std::unique_ptr<StatementNode>&& thenBranch,
        std::unique_ptr<StatementNode>&& elseBranch,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          condition(std::move(conditionDeclaration)),
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}

    const IfStatementNode::Condition& IfStatementNode::getCondition() const noexcept {
        return condition;
    }

    const StatementNode& IfStatementNode::getThenBranch() const noexcept {
        return *thenBranch;
    }

    const StatementNode* IfStatementNode::getElseBranch() const noexcept {
        return elseBranch.get();
    }

    LoopStatementNode::LoopStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          label(std::move(label)) {}

    const IdentifierNode* LoopStatementNode::getLabel() const noexcept {
        return label.get();
    }

    ForStatementNode::ForStatementNode(
        std::unique_ptr<ValueDeclarationNode>&& loopVariable,
        std::unique_ptr<ExpressionNode>&& iterableExpression,
        std::unique_ptr<StatementNode>&& body,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LoopStatementNode(nullptr, firstToken, lastToken),
          loopVariable(std::move(loopVariable)),
          iterableExpression(std::move(iterableExpression)),
          body(std::move(body)) {}

    ForStatementNode::ForStatementNode(
        std::unique_ptr<IdentifierNode>&& label,
        std::unique_ptr<ValueDeclarationNode>&& loopVariable,
        std::unique_ptr<ExpressionNode>&& iterableExpression,
        std::unique_ptr<StatementNode>&& body,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LoopStatementNode(std::move(label), firstToken, lastToken),
          loopVariable(std::move(loopVariable)),
          iterableExpression(std::move(iterableExpression)),
          body(std::move(body)) {}

    const ValueDeclarationNode& ForStatementNode::getLoopVariable() const noexcept {
        return *loopVariable;
    }

    const ExpressionNode& ForStatementNode::getIterableExpression() const noexcept {
        return *iterableExpression;
    }

    const StatementNode& ForStatementNode::getBody() const noexcept {
        return *body;
    }

    WhileStatementNode::WhileStatementNode(
        std::unique_ptr<ExpressionNode>&& conditionExpression,
        std::unique_ptr<StatementNode>&& body,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LoopStatementNode(nullptr, firstToken, lastToken),
          condition(std::move(conditionExpression)),
          body(std::move(body)) {}

    WhileStatementNode::WhileStatementNode(
        std::unique_ptr<ValueDeclarationNode>&& conditionDeclaration,
        std::unique_ptr<StatementNode>&& body,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LoopStatementNode(nullptr, firstToken, lastToken),
          condition(std::move(conditionDeclaration)),
          body(std::move(body)) {}

    WhileStatementNode::WhileStatementNode(
        std::unique_ptr<IdentifierNode>&& label,
        std::unique_ptr<ExpressionNode>&& conditionExpression,
        std::unique_ptr<StatementNode>&& body,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LoopStatementNode(std::move(label), firstToken, lastToken),
          condition(std::move(conditionExpression)),
          body(std::move(body)) {}

    WhileStatementNode::WhileStatementNode(
        std::unique_ptr<IdentifierNode>&& label,
        std::unique_ptr<ValueDeclarationNode>&& conditionDeclaration,
        std::unique_ptr<StatementNode>&& body,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LoopStatementNode(std::move(label), firstToken, lastToken),
          condition(std::move(conditionDeclaration)),
          body(std::move(body)) {}

    const WhileStatementNode::Condition& WhileStatementNode::getCondition() const noexcept {
        return condition;
    }

    const StatementNode& WhileStatementNode::getBody() const noexcept {
        return *body;
    }

    SwitchStatementNode::SwitchStatementNode(
        std::unique_ptr<ExpressionNode>&& conditionExpression,
        std::vector<LiteralMatchItem>&& literalMatchItems,
        std::unique_ptr<StatementNode>&& defaultCaseBody,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          condition(std::move(conditionExpression)),
          items(std::move(literalMatchItems)),
          defaultCaseBody(std::move(defaultCaseBody)) {}

    SwitchStatementNode::SwitchStatementNode(
        std::unique_ptr<ValueDeclarationNode>&& conditionDeclaration,
        std::vector<LiteralMatchItem>&& literalMatchItems,
        std::unique_ptr<StatementNode>&& defaultCaseBody,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          condition(std::move(conditionDeclaration)),
          items(std::move(literalMatchItems)),
          defaultCaseBody(std::move(defaultCaseBody)) {}

    SwitchStatementNode::SwitchStatementNode(
        std::unique_ptr<ExpressionNode>&& conditionExpression,
        std::vector<TypeMatchItem>&& typeMatchItems,
        std::unique_ptr<StatementNode>&& defaultCaseBody,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          condition(std::move(conditionExpression)),
          items(std::move(typeMatchItems)),
          defaultCaseBody(std::move(defaultCaseBody)) {}

    SwitchStatementNode::SwitchStatementNode(
        std::unique_ptr<ValueDeclarationNode>&& conditionDeclaration,
        std::vector<TypeMatchItem>&& typeMatchItems,
        std::unique_ptr<StatementNode>&& defaultCaseBody,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          condition(std::move(conditionDeclaration)),
          items(std::move(typeMatchItems)),
          defaultCaseBody(std::move(defaultCaseBody)) {}

    const SwitchStatementNode::Condition& SwitchStatementNode::getCondition() const noexcept {
        return condition;
    }

    const SwitchStatementNode::Items& SwitchStatementNode::getItems() const noexcept {
        return items;
    }

    const StatementNode* SwitchStatementNode::getDefaultCaseBody() const noexcept {
        return defaultCaseBody.get();
    }

    ReturnStatementNode::ReturnStatementNode(const Token& firstToken, const Token& lastToken) noexcept : ControlFlowStatementNode(firstToken, lastToken), returnValue(nullptr) {}

    ReturnStatementNode::ReturnStatementNode(std::unique_ptr<ExpressionNode>&& returnValue, const Token& firstToken, const Token& lastToken) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          returnValue(std::move(returnValue)) {}

    const ExpressionNode* ReturnStatementNode::getReturnValue() const noexcept {
        return returnValue.get();
    }

    BreakStatementNode::BreakStatementNode(const Token& firstToken, const Token& lastToken) noexcept : ControlFlowStatementNode(firstToken, lastToken), label(nullptr) {}

    BreakStatementNode::BreakStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          label(std::move(label)) {}

    const IdentifierNode* BreakStatementNode::getLabel() const noexcept {
        return label.get();
    }

    ContinueStatementNode::ContinueStatementNode(const Token& firstToken, const Token& lastToken) noexcept : ControlFlowStatementNode(firstToken, lastToken), label(nullptr) {}

    ContinueStatementNode::ContinueStatementNode(std::unique_ptr<IdentifierNode>&& label, const Token& firstToken, const Token& lastToken) noexcept
        : ControlFlowStatementNode(firstToken, lastToken),
          label(std::move(label)) {}

    const IdentifierNode* ContinueStatementNode::getLabel() const noexcept {
        return label.get();
    }

    ReloadStatementNode::ReloadStatementNode(const Token& firstToken, const Token& lastToken) noexcept : ControlFlowStatementNode(firstToken, lastToken) {}
} // namespace vnlc
