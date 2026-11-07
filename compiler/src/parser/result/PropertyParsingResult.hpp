#ifndef VNLC_PROPERTY_PARSING_RESULT_HPP
#define VNLC_PROPERTY_PARSING_RESULT_HPP

#include "ast/declaration/ValueDeclarationNode.hpp"
#include <memory>

namespace vnlc {
    struct PropertyParsingResult {
        std::unique_ptr<ValueDeclarationNode> declaration;
    };
} // namespace vnlc

#endif // VNLC_PROPERTY_PARSING_RESULT_HPP
