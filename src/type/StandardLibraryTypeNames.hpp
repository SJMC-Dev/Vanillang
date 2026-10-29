#ifndef VNLC_STANDARD_LIBRARY_TYPE_NAMES_HPP
#define VNLC_STANDARD_LIBRARY_TYPE_NAMES_HPP

#include <fmt/compile.h>
#include <string_view>

namespace vnlc {
    namespace StandardLibraryTypeNames {
        constexpr std::string_view Any = "vanillang.typesystem.Any";
        constexpr std::string Optional(std::string_view T) {
            return fmt::format(FMT_COMPILE("vanillang.typesystem.Optional<{}>"), T);
        }
        constexpr std::string Result(std::string_view T) {
            return fmt::format(FMT_COMPILE("vanillang.typesystem.Result<{}>"), T);
        }
        constexpr std::string Iterator(std::string_view T) {
            return fmt::format(FMT_COMPILE("vanillang.typesystem.Iterator<{}>"), T);
        }
        constexpr std::string Iterable(std::string_view T) {
            return fmt::format(FMT_COMPILE("vanillang.typesystem.Iterable<{}>"), T);
        }
        constexpr std::string Range(std::string_view T) {
            return fmt::format(FMT_COMPILE("vanillang.typesystem.Range<{}>"), T);
        }
        constexpr std::string Array(std::string_view T) {
            return fmt::format(FMT_COMPILE("vanillang.typesystem.Array<{}>"), T);
        }
        constexpr std::string_view List = "vanillang.typesystem.List";
        constexpr std::string_view Dict = "vanillang.typesystem.Dict";
        constexpr std::string_view Selector = "vanillang.typesystem.Selector";
        constexpr std::string_view Ordering = "vanillang.typesystem.Ordering";
    } // namespace StandardLibraryTypeNames
} // namespace vnlc

#endif // VNLC_STANDARD_LIBRARY_TYPE_NAMES_HPP
