#ifndef VNLC_PRIMITIVE_TYPE_NAMES_HPP
#define VNLC_PRIMITIVE_TYPE_NAMES_HPP

#include <string_view>

namespace vnlc {
    namespace PrimitiveTypeNames {
        constexpr std::string_view Byte = "byte";
        constexpr std::string_view Short = "short";
        constexpr std::string_view Int = "int";
        constexpr std::string_view Long = "long";
        constexpr std::string_view Float = "float";
        constexpr std::string_view Double = "double";
        constexpr std::string_view Boolean = "bool";
        constexpr std::string_view String = "string";
    } // namespace PrimitiveTypeNames
} // namespace vnlc

#endif // VNLC_PRIMITIVE_TYPE_NAMES_HPP
