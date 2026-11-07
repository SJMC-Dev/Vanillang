#ifndef VNLC_FUNCTION_SIGNATURE_PARSING_RESULT_HPP
#define VNLC_FUNCTION_SIGNATURE_PARSING_RESULT_HPP

#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/typeannotation/TypeAnnotationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct FunctionSignatureParsingResult {
        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<ValueDeclarationNode>> parameters;
        std::unique_ptr<TypeAnnotationNode> returnType; // nullptr if the return type is omitted
    };
} // namespace vnlc

#endif // VNLC_FUNCTION_SIGNATURE_PARSING_RESULT_HPP