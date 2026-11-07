#pragma once

#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/typeannotation/TypeAnnotationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct FunctionHeaderParsingResult {
        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<ValueDeclarationNode>> parameters;
        std::unique_ptr<TypeAnnotationNode> returnType; // nullptr if the return type is omitted
    };
} // namespace vnlc
