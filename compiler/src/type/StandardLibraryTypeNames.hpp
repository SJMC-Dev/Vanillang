#pragma once

#include <fmt/compile.h>
#include <string_view>

namespace vnlc {
    namespace StandardLibraryTypeNames {
        constexpr std::string_view Any = "vanillang.types.Any";
        constexpr std::string Optional(std::string_view T) {
            return fmt::format(FMT_COMPILE("vanillang.types.Optional<{}>"), T);
        }
        constexpr std::string Result(std::string_view T) {
            return fmt::format(FMT_COMPILE("vanillang.types.Result<{}>"), T);
        }
        constexpr std::string Iterator(std::string_view T) {
            return fmt::format(FMT_COMPILE("vanillang.types.Iterator<{}>"), T);
        }
        constexpr std::string Iterable(std::string_view T) {
            return fmt::format(FMT_COMPILE("vanillang.types.Iterable<{}>"), T);
        }
        constexpr std::string Range(std::string_view T) {
            return fmt::format(FMT_COMPILE("vanillang.types.Range<{}>"), T);
        }
        constexpr std::string Array(std::string_view T) {
            return fmt::format(FMT_COMPILE("vanillang.types.Array<{}>"), T);
        }
        constexpr std::string_view String = "vanillang.types.String";
        constexpr std::string_view List = "vanillang.types.List";
        constexpr std::string_view Dict = "vanillang.types.Dict";
        constexpr std::string_view Selector = "vanillang.types.Selector";
        constexpr std::string_view Ordering = "vanillang.types.Ordering";
    }; // namespace StandardLibraryTypeNames
} // namespace vnlc
