#ifndef VNLC_FUNCTION_CALL_EXPRESSION_NODE_HPP
#define VNLC_FUNCTION_CALL_EXPRESSION_NODE_HPP

#include "ast/expression/ExpressionNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class FunctionCallExpressionNode : public ExpressionNode {
    private:
        std::unique_ptr<ExpressionNode> callee;
        std::vector<std::unique_ptr<ExpressionNode>> arguments;
        std::unique_ptr<ExpressionNode> context; // nullptr if no context argument

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
            std::unique_ptr<ExpressionNode>&& context,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const ExpressionNode& getCallee() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ExpressionNode>>& getArguments() const noexcept;
        [[nodiscard]] const ExpressionNode* getContext() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_FUNCTION_CALL_EXPRESSION_NODE_HPP
