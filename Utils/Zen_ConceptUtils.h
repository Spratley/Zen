#pragma once

#include <type_traits>

namespace Zen
{
    namespace ConceptUtils
    {
        template <typename Type>
        concept IsPureType = std::is_same_v<Type, std::remove_cvref_t<Type>> && !std::is_pointer_v<Type>;
        // A pure type is any type that has no modifiers attached (i.e. const, *, or &)

        template <typename Type>
        concept IsIndirectType = std::is_pointer_v<Type> || std::is_reference_v<Type>;
    } // namespace ConceptUtils
} // namespace Zen