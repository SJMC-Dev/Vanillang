#ifndef VNLC_TYPE_PARSING_RESULT_HPP
#define VNLC_TYPE_PARSING_RESULT_HPP

#include "ast/typeannotation/TypeAnnotationNode.hpp"
#include <memory>

namespace vnlc {
    struct TypeParsingResult {
        std::unique_ptr<TypeAnnotationNode> type;
    };
} // namespace vnlc

#endif // VNLC_TYPE_PARSING_RESULT_HPP
