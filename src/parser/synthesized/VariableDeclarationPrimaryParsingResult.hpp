#ifndef VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_RESULT_HPP
#define VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_RESULT_HPP

#include "ast/declaration/ValueDeclarationKind.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include <memory>

namespace vnlc {
    struct VariableDeclarationPrimaryParsingResult {
        ValueDeclarationKind::Kind kind;
        std::unique_ptr<IdentifierNode> name;
        std::unique_ptr<TypeReferenceNode> type; // nullptr if no type annotation
    };
} // namespace vnlc

#endif // VNLC_VARIABLE_DECLARATION_PRIMARY_PARSING_RESULT_HPP