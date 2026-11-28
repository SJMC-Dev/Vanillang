#include "ExpressionNode.hpp"
#include <utility>

namespace vnlc {
    ExpressionNode::ExpressionNode(const Token& firstToken, const Token& lastToken) : AstNode(firstToken, lastToken) {}

    PrimaryExpressionNode::PrimaryExpressionNode(const Token& firstToken, const Token& lastToken) noexcept : ExpressionNode(firstToken, lastToken) {}

    IdentifierLikeExpressionNode::IdentifierLikeExpressionNode(std::unique_ptr<IdentifierNode>&& name, const Token& firstToken, const Token& lastToken) noexcept
        : PrimaryExpressionNode(firstToken, lastToken),
          name(std::move(name)),
          genericArguments() {}

    IdentifierLikeExpressionNode::IdentifierLikeExpressionNode(
        std::unique_ptr<IdentifierNode>&& name,
        std::vector<std::unique_ptr<TypeAnnotationNode>>&& genericArguments,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : PrimaryExpressionNode(firstToken, lastToken),
          name(std::move(name)),
          genericArguments(std::move(genericArguments)) {}

    const IdentifierNode& IdentifierLikeExpressionNode::getName() const noexcept {
        return *name;
    }

    const std::vector<std::unique_ptr<TypeAnnotationNode>>& IdentifierLikeExpressionNode::getGenericArguments() const noexcept {
        return genericArguments;
    }

    KeywordExpressionNode::KeywordExpressionNode(ExpressionKeyword keyword, const Token& firstToken, const Token& lastToken) noexcept
        : PrimaryExpressionNode(firstToken, lastToken),
          keyword(keyword) {}

    ExpressionKeyword KeywordExpressionNode::getKeyword() const noexcept {
        return keyword;
    }

    LiteralExpressionNode::LiteralExpressionNode(const Token& firstToken, const Token& lastToken) noexcept : PrimaryExpressionNode(firstToken, lastToken) {}

    SimpleLiteralExpressionNode::SimpleLiteralExpressionNode(PrimitiveTypeKind type, std::string_view literal, const Token& firstToken, const Token& lastToken) noexcept
        : LiteralExpressionNode(firstToken, lastToken),
          type(type),
          literal(std::string(literal)) {}

    PrimitiveTypeKind SimpleLiteralExpressionNode::getType() const noexcept {
        return type;
    }

    std::string_view SimpleLiteralExpressionNode::getLiteral() const noexcept {
        return literal;
    }

    StringLiteralExpressionNode::StringLiteralExpressionNode(
        StringLiteralKind kind,
        std::vector<std::variant<std::string, std::unique_ptr<ExpressionNode>>>&& parts,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LiteralExpressionNode(firstToken, lastToken),
          kind(kind),
          parts(std::move(parts)) {}

    StringLiteralKind StringLiteralExpressionNode::getKind() const noexcept {
        return kind;
    }

    const std::vector<std::variant<std::string, std::unique_ptr<ExpressionNode>>>& StringLiteralExpressionNode::getParts() const noexcept {
        return parts;
    }

    ListLikeLiteralExpressionNode::ListLikeLiteralExpressionNode(
        ListLikeLiteralKind kind,
        std::vector<std::unique_ptr<ExpressionNode>>&& elements,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : LiteralExpressionNode(firstToken, lastToken),
          kind(kind),
          elements(std::move(elements)) {}

    ListLikeLiteralKind ListLikeLiteralExpressionNode::getKind() const noexcept {
        return kind;
    }

    const std::vector<std::unique_ptr<ExpressionNode>>& ListLikeLiteralExpressionNode::getElements() const noexcept {
        return elements;
    }

    DictLiteralExpressionNode::DictLiteralExpressionNode(std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>&& entries, const Token& firstToken, const Token& lastToken) noexcept
        : LiteralExpressionNode(firstToken, lastToken),
          entries(std::move(entries)) {}

    const std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>& DictLiteralExpressionNode::getEntries() const noexcept {
        return entries;
    }

    SelectorLiteralExpressionNode::SelectorLiteralExpressionNode(SelectorKind kind, std::vector<SelectorArgumentTerm>&& arguments, const Token& firstToken, const Token& lastToken) noexcept
        : LiteralExpressionNode(firstToken, lastToken),
          kind(kind),
          arguments(std::move(arguments)) {}

    SelectorKind SelectorLiteralExpressionNode::getKind() const noexcept {
        return kind;
    }

    const std::vector<SelectorArgumentTerm>& SelectorLiteralExpressionNode::getArguments() const noexcept {
        return arguments;
    }

    UnaryExpressionNode::UnaryExpressionNode(UnaryExpressionKind kind, std::unique_ptr<ExpressionNode>&& operand, const Token& firstToken, const Token& lastToken) noexcept
        : ExpressionNode(firstToken, lastToken),
          kind(kind),
          operand(std::move(operand)) {}

    UnaryExpressionKind UnaryExpressionNode::getKind() const noexcept {
        return kind;
    }

    const ExpressionNode& UnaryExpressionNode::getOperand() const noexcept {
        return *operand;
    }

    BinaryExpressionNode::BinaryExpressionNode(
        BinaryExpressionKind kind,
        std::unique_ptr<ExpressionNode>&& left,
        std::unique_ptr<ExpressionNode>&& right,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          kind(kind),
          left(std::move(left)),
          right(std::move(right)) {}

    BinaryExpressionKind BinaryExpressionNode::getKind() const noexcept {
        return kind;
    }

    const ExpressionNode& BinaryExpressionNode::getLeft() const noexcept {
        return *left;
    }

    const ExpressionNode& BinaryExpressionNode::getRight() const noexcept {
        return *right;
    }

    RangeExpressionNode::RangeExpressionNode(std::unique_ptr<ExpressionNode>&& start, std::unique_ptr<ExpressionNode>&& end, const Token& firstToken, const Token& lastToken) noexcept
        : ExpressionNode(firstToken, lastToken),
          start(std::move(start)),
          end(std::move(end)) {}

    const ExpressionNode* RangeExpressionNode::getStart() const noexcept {
        return start.get();
    }

    const ExpressionNode* RangeExpressionNode::getEnd() const noexcept {
        return end.get();
    }

    InstanceofExpressionNode::InstanceofExpressionNode(
        std::unique_ptr<ExpressionNode>&& expression,
        std::unique_ptr<TypeAnnotationNode>&& type,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          expression(std::move(expression)),
          type(std::move(type)) {}

    const ExpressionNode& InstanceofExpressionNode::getExpression() const noexcept {
        return *expression;
    }

    const TypeAnnotationNode& InstanceofExpressionNode::getType() const noexcept {
        return *type;
    }

    ConditionalExpressionNode::ConditionalExpressionNode(
        std::unique_ptr<ExpressionNode>&& condition,
        std::unique_ptr<ExpressionNode>&& thenExpression,
        std::unique_ptr<ExpressionNode>&& elseExpression,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          condition(std::move(condition)),
          thenExpression(std::move(thenExpression)),
          elseExpression(std::move(elseExpression)) {}

    const ExpressionNode& ConditionalExpressionNode::getCondition() const noexcept {
        return *condition;
    }

    const ExpressionNode& ConditionalExpressionNode::getThenExpression() const noexcept {
        return *thenExpression;
    }

    const ExpressionNode& ConditionalExpressionNode::getElseExpression() const noexcept {
        return *elseExpression;
    }

    FunctionCallExpressionNode::FunctionCallExpressionNode(
        std::unique_ptr<ExpressionNode>&& callee,
        std::vector<std::unique_ptr<ExpressionNode>>&& arguments,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          callee(std::move(callee)),
          arguments(std::move(arguments)),
          context(nullptr) {}

    FunctionCallExpressionNode::FunctionCallExpressionNode(
        std::unique_ptr<ExpressionNode>&& callee,
        std::vector<std::unique_ptr<ExpressionNode>>&& arguments,
        std::unique_ptr<DictLiteralExpressionNode>&& context,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          callee(std::move(callee)),
          arguments(std::move(arguments)),
          context(std::move(context)) {}

    const ExpressionNode& FunctionCallExpressionNode::getCallee() const noexcept {
        return *callee;
    }

    const std::vector<std::unique_ptr<ExpressionNode>>& FunctionCallExpressionNode::getArguments() const noexcept {
        return arguments;
    }

    const DictLiteralExpressionNode* FunctionCallExpressionNode::getContext() const noexcept {
        return context.get();
    }

    SubscriptExpressionNode::SubscriptExpressionNode(
        std::unique_ptr<ExpressionNode>&& object,
        std::unique_ptr<ExpressionNode>&& index,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          object(std::move(object)),
          index(std::move(index)) {}

    const ExpressionNode& SubscriptExpressionNode::getObject() const noexcept {
        return *object;
    }

    const ExpressionNode& SubscriptExpressionNode::getIndex() const noexcept {
        return *index;
    }

    MemberAccessExpressionNode::MemberAccessExpressionNode(
        std::unique_ptr<ExpressionNode>&& object,
        std::unique_ptr<IdentifierLikeExpressionNode>&& member,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : ExpressionNode(firstToken, lastToken),
          object(std::move(object)),
          member(std::move(member)) {}

    const ExpressionNode& MemberAccessExpressionNode::getObject() const noexcept {
        return *object;
    }

    const IdentifierLikeExpressionNode& MemberAccessExpressionNode::getMember() const noexcept {
        return *member;
    }
} // namespace vnlc
