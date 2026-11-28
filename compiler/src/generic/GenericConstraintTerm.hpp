#pragma once

#include "identifier/IdentifierNode.hpp"
#include "prop/LanguageProperty.hpp"
#include "type/TypeAnnotationNode.hpp"
#include <memory>
#include <variant>
#include <vector>

namespace vnlc {
    struct GenericConstraintTerm {
        struct NullableConstraint {};

        struct InheritanceConstraint {
            std::unique_ptr<TypeAnnotationNode> baseClass;
        };

        struct ImplementationConstraint {
            std::vector<std::unique_ptr<TypeAnnotationNode>> implementedInterfaces;
        };

        struct OperatorConstraint {
            OperatorName op;
            std::vector<std::unique_ptr<TypeAnnotationNode>> parameters; // empty if no parameter
            std::unique_ptr<TypeAnnotationNode> returnType;
        };

        std::unique_ptr<IdentifierNode> name;
        std::variant<NullableConstraint, InheritanceConstraint, ImplementationConstraint, OperatorConstraint> constraint;
    };

} // namespace vnlc