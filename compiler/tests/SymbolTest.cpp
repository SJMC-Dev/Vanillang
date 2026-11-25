#include "symbol/RegularSymbol.hpp"
#include <gtest/gtest.h>

namespace vnlc {
    TEST(RegularSymbolTest, StoresKindOriginAccessModifierAndNodes) {
        const RegularSymbol symbol(RegularSymbolKind::PROPERTY, RegularSymbolAccessModifier::PROTECTED, "count", static_cast<const AstNode*>(nullptr));

        EXPECT_EQ(symbol.getKind(), RegularSymbolKind::PROPERTY);
        EXPECT_EQ(symbol.getOrigin(), RegularSymbolOrigin::LOCAL);
        EXPECT_EQ(symbol.getAccessModifier(), RegularSymbolAccessModifier::PROTECTED);
        EXPECT_EQ(symbol.getName(), "count");
        EXPECT_EQ(symbol.getLocalNode(), nullptr);
        EXPECT_EQ(symbol.getImportedNode(), nullptr);
    }

} // namespace vnlc
