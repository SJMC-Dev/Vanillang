#pragma once

#include "ast/AstNode.hpp"
#include "identifier/IdentifierNode.hpp"
#include "prop/LanguageProperty.hpp"
#include "selector/SelectorArgumentTerm.hpp"
#include "token/Token.hpp"
#include "type/TypeAnnotationNode.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace vnlc {
    class ExpressionNode : public AstNode {
    protected:
        ExpressionNode(const Token& firstToken, const Token& lastToken);

    public:
        ExpressionNode() = delete;
    };

    class PrimaryExpressionNode : public ExpressionNode {
    protected:
        PrimaryExpressionNode(const Token& firstToken, const Token& lastToken) noexcept;

    public:
        PrimaryExpressionNode() = delete;
    };

    class IdentifierLikeExpressionNode : public PrimaryExpressionNode {
    private:
        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<TypeAnnotationNode>> genericArguments;

    public:
        IdentifierLikeExpressionNode() = delete;

        IdentifierLikeExpressionNode(std::unique_ptr<IdentifierNode>&& name, const Token& firstToken, const Token& lastToken) noexcept;
        IdentifierLikeExpressionNode(
            std::unique_ptr<IdentifierNode>&& name,
            std::vector<std::unique_ptr<TypeAnnotationNode>>&& genericArguments,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<TypeAnnotationNode>>& getGenericArguments() const noexcept;
    };

    class KeywordExpressionNode : public PrimaryExpressionNode {
    private:
        ExpressionKeyword keyword;

    public:
        KeywordExpressionNode() = delete;

        KeywordExpressionNode(ExpressionKeyword keyword, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] ExpressionKeyword getKeyword() const noexcept;
    };

    class LiteralExpressionNode : public PrimaryExpressionNode {
    protected:
        LiteralExpressionNode(const Token& firstToken, const Token& lastToken) noexcept;

    public:
        LiteralExpressionNode() = delete;
    };

    class SimpleLiteralExpressionNode : public LiteralExpressionNode {
    private:
        PrimitiveTypeKind type;
        std::string literal;

    public:
        SimpleLiteralExpressionNode() = delete;

        SimpleLiteralExpressionNode(PrimitiveTypeKind type, std::string_view literal, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] PrimitiveTypeKind getType() const noexcept;
        [[nodiscard]] std::string_view getLiteral() const noexcept;
    };

    class StringLiteralExpressionNode : public LiteralExpressionNode {
    private:
        StringLiteralKind kind;
        std::vector<std::variant<std::string, std::unique_ptr<ExpressionNode>>> parts;

    public:
        StringLiteralExpressionNode() = delete;

        StringLiteralExpressionNode(
            StringLiteralKind kind,
            std::vector<std::variant<std::string, std::unique_ptr<ExpressionNode>>>&& parts,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] StringLiteralKind getKind() const noexcept;
        [[nodiscard]] const std::vector<std::variant<std::string, std::unique_ptr<ExpressionNode>>>& getParts() const noexcept;
    };

    class ListLikeLiteralExpressionNode : public LiteralExpressionNode {
    private:
        ListLikeLiteralKind kind;
        std::vector<std::unique_ptr<ExpressionNode>> elements;

    public:
        ListLikeLiteralExpressionNode() = delete;

        ListLikeLiteralExpressionNode(ListLikeLiteralKind kind, std::vector<std::unique_ptr<ExpressionNode>>&& elements, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] ListLikeLiteralKind getKind() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ExpressionNode>>& getElements() const noexcept;
    };

    class DictLiteralExpressionNode : public LiteralExpressionNode {
    private:
        std::unordered_map<std::string, std::unique_ptr<ExpressionNode>> entries;

    public:
        DictLiteralExpressionNode() = delete;

        DictLiteralExpressionNode(std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>&& entries, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>& getEntries() const noexcept;
    };

    class SelectorLiteralExpressionNode : public LiteralExpressionNode {
    private:
        SelectorKind kind;
        std::vector<SelectorArgumentTerm> arguments; // empty if no argument

    public:
        SelectorLiteralExpressionNode() = delete;

        SelectorLiteralExpressionNode(SelectorKind kind, std::vector<SelectorArgumentTerm>&& arguments, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] SelectorKind getKind() const noexcept;
        [[nodiscard]] const std::vector<SelectorArgumentTerm>& getArguments() const noexcept;
    };

    class UnaryExpressionNode : public ExpressionNode {
    private:
        UnaryExpressionKind kind;
        std::unique_ptr<ExpressionNode> operand;

    public:
        UnaryExpressionNode() = delete;

        UnaryExpressionNode(UnaryExpressionKind kind, std::unique_ptr<ExpressionNode>&& operand, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] UnaryExpressionKind getKind() const noexcept;
        [[nodiscard]] const ExpressionNode& getOperand() const noexcept;
    };

    class BinaryExpressionNode : public ExpressionNode {
    private:
        BinaryExpressionKind kind;

        std::unique_ptr<ExpressionNode> left;
        std::unique_ptr<ExpressionNode> right;

    public:
        BinaryExpressionNode() = delete;

        BinaryExpressionNode(
            BinaryExpressionKind kind,
            std::unique_ptr<ExpressionNode>&& left,
            std::unique_ptr<ExpressionNode>&& right,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] BinaryExpressionKind getKind() const noexcept;
        [[nodiscard]] const ExpressionNode& getLeft() const noexcept;
        [[nodiscard]] const ExpressionNode& getRight() const noexcept;
    };

    class RangeExpressionNode : public ExpressionNode {
    private:
        std::unique_ptr<ExpressionNode> start; // nullptr if no start bound
        std::unique_ptr<ExpressionNode> end;   // nullptr if no end bound

    public:
        RangeExpressionNode() = delete;

        RangeExpressionNode(std::unique_ptr<ExpressionNode>&& start, std::unique_ptr<ExpressionNode>&& end, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ExpressionNode* getStart() const noexcept;
        [[nodiscard]] const ExpressionNode* getEnd() const noexcept;
    };

    class InstanceofExpressionNode : public ExpressionNode {
    private:
        std::unique_ptr<ExpressionNode> expression;
        std::unique_ptr<TypeAnnotationNode> type;

    public:
        InstanceofExpressionNode() = delete;

        InstanceofExpressionNode(std::unique_ptr<ExpressionNode>&& expression, std::unique_ptr<TypeAnnotationNode>&& type, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ExpressionNode& getExpression() const noexcept;
        [[nodiscard]] const TypeAnnotationNode& getType() const noexcept;
    };

    class ConditionalExpressionNode : public ExpressionNode {
    private:
        std::unique_ptr<ExpressionNode> condition;
        std::unique_ptr<ExpressionNode> thenExpression;
        std::unique_ptr<ExpressionNode> elseExpression;

    public:
        ConditionalExpressionNode() = delete;

        ConditionalExpressionNode(
            std::unique_ptr<ExpressionNode>&& condition,
            std::unique_ptr<ExpressionNode>&& thenExpression,
            std::unique_ptr<ExpressionNode>&& elseExpression,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const ExpressionNode& getCondition() const noexcept;
        [[nodiscard]] const ExpressionNode& getThenExpression() const noexcept;
        [[nodiscard]] const ExpressionNode& getElseExpression() const noexcept;
    };

    class FunctionCallExpressionNode : public ExpressionNode {
    private:
        std::unique_ptr<ExpressionNode> callee;
        std::vector<std::unique_ptr<ExpressionNode>> arguments;
        std::unique_ptr<DictLiteralExpressionNode> context; // nullptr if no context argument

    public:
        FunctionCallExpressionNode() = delete;

        FunctionCallExpressionNode(
            std::unique_ptr<ExpressionNode>&& callee,
            std::vector<std::unique_ptr<ExpressionNode>>&& arguments,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        FunctionCallExpressionNode(
            std::unique_ptr<ExpressionNode>&& callee,
            std::vector<std::unique_ptr<ExpressionNode>>&& arguments,
            std::unique_ptr<DictLiteralExpressionNode>&& context,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const ExpressionNode& getCallee() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ExpressionNode>>& getArguments() const noexcept;
        [[nodiscard]] const DictLiteralExpressionNode* getContext() const noexcept;
    };

    class SubscriptExpressionNode : public ExpressionNode {
    private:
        std::unique_ptr<ExpressionNode> object;
        std::unique_ptr<ExpressionNode> index;

    public:
        SubscriptExpressionNode() = delete;

        SubscriptExpressionNode(std::unique_ptr<ExpressionNode>&& object, std::unique_ptr<ExpressionNode>&& index, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ExpressionNode& getObject() const noexcept;
        [[nodiscard]] const ExpressionNode& getIndex() const noexcept;
    };

    class MemberAccessExpressionNode : public ExpressionNode {
    private:
        std::unique_ptr<ExpressionNode> object;
        std::unique_ptr<IdentifierLikeExpressionNode> member;

    public:
        MemberAccessExpressionNode() = delete;

        MemberAccessExpressionNode(std::unique_ptr<ExpressionNode>&& object, std::unique_ptr<IdentifierLikeExpressionNode>&& member, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ExpressionNode& getObject() const noexcept;
        [[nodiscard]] const IdentifierLikeExpressionNode& getMember() const noexcept;
    };
} // namespace vnlc
