#pragma once

#include "../Zen_Types.h"

#include <type_traits>

namespace Zen
{
    // TypeList is an empty object type that smuggles other types as template parameters. Originally I had used a Tuple
    // but found myself needing to instantiate the tuple to call constructors. This way, instantiating a TypeList to
    // call a templated constructor will always be a simple call, even if complex types are used as parameters.
    template <typename... Types>
    struct TypeList
    {};

    namespace TypeListUtils
    {
        // -=-=-=-= TypeList Concat =-=-=-=-
        template <typename ATypes, typename BTypes, typename... RemainingTypes>
        struct Concat;

        template <typename... ATypes, typename... BTypes>
        struct Concat<TypeList<ATypes...>, TypeList<BTypes...>>
        {
            using Type = TypeList<ATypes..., BTypes...>;
        };

        template <typename... ATypes, typename... BTypes, typename... RemainingTypes>
        struct Concat<TypeList<ATypes...>, TypeList<BTypes...>, RemainingTypes...>
        {
            using Type = typename Concat<TypeList<ATypes..., BTypes...>, RemainingTypes...>::Type;
        };

        template <typename... TypeLists>
        using Concat_T = typename Concat<TypeLists...>::Type;

        // -=-=-=-= Contains Type =-=-=-=-
        template <typename T, typename Types>
        struct ContainsType;

        template <typename T, typename... Types>
        struct ContainsType<T, TypeList<Types...>> : std::bool_constant<(std::is_same_v<T, Types> || ...)>
        {};

        template <typename T, typename... Types>
        constexpr inline bool ContainsType_V = ContainsType<T, Types...>::value;

        // -=-=-=-= Index Of =-=-=-=-
        template <typename T, typename Types>
        struct IndexOf;

        template <typename T, typename... Types>
        struct IndexOf<T, TypeList<T, Types...>> : std::integral_constant<SizeT, 0>
        {};

        template <typename T, typename U, typename... Types>
        struct IndexOf<T, TypeList<U, Types...>> : std::integral_constant<SizeT, IndexOf<T, TypeList<Types...>>::value + 1>
        {};

        template <typename T, typename... Types>
        constexpr inline SizeT IndexOf_V = IndexOf<T, Types...>::value;

        // -=-=-=-= Filter Duplicates =-=-=-=-
        template <typename RemainingTypes, typename FilteredTypes = TypeList<>>
        struct FilterDuplicates;

        template <bool IsDuplicate, typename T, typename RemainingTypes, typename FilteredTypes>
        struct FilterDuplicatesBranch;

        template <typename FilteredTypes>
        struct FilterDuplicates<TypeList<>, FilteredTypes>
        {
            using Type = FilteredTypes;
        };

        template <typename T, typename... RemainingTypes, typename... FilteredTypes>
        struct FilterDuplicates<TypeList<T, RemainingTypes...>, TypeList<FilteredTypes...>>
        {
            static constexpr bool IsDuplicate = ContainsType_V<T, TypeList<FilteredTypes...>>;
            using Type =
              typename FilterDuplicatesBranch<IsDuplicate, T, TypeList<RemainingTypes...>, TypeList<FilteredTypes...>>::Type;
        };

        template <typename T, typename... RemainingTypes, typename... FilteredTypes>
        struct FilterDuplicatesBranch<true, T, TypeList<RemainingTypes...>, TypeList<FilteredTypes...>>
        {
            using Type = typename FilterDuplicates<TypeList<RemainingTypes...>, TypeList<FilteredTypes...>>::Type;
        };

        template <typename T, typename... RemainingTypes, typename... FilteredTypes>
        struct FilterDuplicatesBranch<false, T, TypeList<RemainingTypes...>, TypeList<FilteredTypes...>>
        {
            using Type = typename FilterDuplicates<TypeList<RemainingTypes...>, TypeList<FilteredTypes..., T>>::Type;
        };

        template <typename Types>
        using FilterDuplicates_T = typename FilterDuplicates<Types>::Type;

        // -=-=-=-= Sort Types =-=-=-=-
        template <template <typename, typename> class Comparator, typename RemainingTypes>
        struct SortTypes;

        template <template <typename, typename> class Comparator,
                  typename RemainingTypes,
                  typename Pivot,
                  typename LHSTypes,
                  typename RHSTypes>
        struct SortTypesImpl;

        template <template <typename, typename> class Comparator,
                  bool IsLHSType,
                  typename T,
                  typename RemainingTypes,
                  typename Pivot,
                  typename LHSTypes,
                  typename RHSTypes>
        struct SortTypesBranch;

        template <template <typename, typename> class Comparator>
        struct SortTypes<Comparator, TypeList<>>
        {
            using Type = TypeList<>;
        };

        template <template <typename, typename> class Comparator, typename T>
        struct SortTypes<Comparator, TypeList<T>>
        {
            using Type = TypeList<T>;
        };

        template <template <typename, typename> class Comparator, typename T, typename... RemainingTypes>
        struct SortTypes<Comparator, TypeList<T, RemainingTypes...>>
        {
            using Type = typename SortTypesImpl<Comparator, TypeList<RemainingTypes...>, T, TypeList<>, TypeList<>>::Type;
        };

        template <template <typename, typename> class Comparator,
                  typename Pivot,
                  typename... LHSTypes,
                  typename... RHSTypes>
        struct SortTypesImpl<Comparator, TypeList<>, Pivot, TypeList<LHSTypes...>, TypeList<RHSTypes...>>
        {
            using Type = typename Concat<typename SortTypes<Comparator, TypeList<LHSTypes...>>::Type,
                                         TypeList<Pivot>,
                                         typename SortTypes<Comparator, TypeList<RHSTypes...>>::Type>::Type;
        };

        template <template <typename, typename> class Comparator,
                  typename T,
                  typename... RemainingTypes,
                  typename Pivot,
                  typename... LHSTypes,
                  typename... RHSTypes>
        struct SortTypesImpl<Comparator, TypeList<T, RemainingTypes...>, Pivot, TypeList<LHSTypes...>, TypeList<RHSTypes...>>
        {
            using Type = typename SortTypesBranch<Comparator,
                                                  Comparator<T, Pivot>::value,
                                                  T,
                                                  TypeList<RemainingTypes...>,
                                                  Pivot,
                                                  TypeList<LHSTypes...>,
                                                  TypeList<RHSTypes...>>::Type;
        };

        template <template <typename, typename> class Comparator,
                  typename T,
                  typename... RemainingTypes,
                  typename Pivot,
                  typename... LHSTypes,
                  typename... RHSTypes>
        struct SortTypesBranch<Comparator,
                               true,
                               T,
                               TypeList<RemainingTypes...>,
                               Pivot,
                               TypeList<LHSTypes...>,
                               TypeList<RHSTypes...>>
        {
            using Type = typename SortTypesImpl<Comparator,
                                                TypeList<RemainingTypes...>,
                                                Pivot,
                                                TypeList<LHSTypes..., T>,
                                                TypeList<RHSTypes...>>::Type;
        };

        template <template <typename, typename> class Comparator,
                  typename T,
                  typename... RemainingTypes,
                  typename Pivot,
                  typename... LHSTypes,
                  typename... RHSTypes>
        struct SortTypesBranch<Comparator,
                               false,
                               T,
                               TypeList<RemainingTypes...>,
                               Pivot,
                               TypeList<LHSTypes...>,
                               TypeList<RHSTypes...>>
        {
            using Type = typename SortTypesImpl<Comparator,
                                                TypeList<RemainingTypes...>,
                                                Pivot,
                                                TypeList<LHSTypes...>,
                                                TypeList<RHSTypes..., T>>::Type;
        };

        template <template <typename, typename> class Comparator, typename Types>
        using SortTypes_T = typename SortTypes<Comparator, Types>::Type;

        // -=-=-=-= Sorting Comparators =-=-=-=-
        template <typename LHS, typename RHS>
        struct CompareLargerAlignment
        {
            static constexpr bool value = alignof(LHS) > alignof(RHS);
        };

    } // namespace TypeListUtils
} // namespace Zen

#include "Zen_TypeListUtils_Tests.inl"