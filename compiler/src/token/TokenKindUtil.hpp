#pragma once

#include "token/TokenKind.hpp"
#include <string_view>

namespace vnlc {
    namespace TokenKindUtil {
        [[nodiscard]] std::string_view toString(TokenKind kind);
        bool isGeneralizedIdentifier(TokenKind kind);
    }; // namespace TokenKindUtil
} // namespace vnlc
