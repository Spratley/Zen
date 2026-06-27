#pragma once

#include <tuple>

namespace Zen
{
    // Should this move to a dedicated Zen_Tuple.h?
    template <typename... Types>
    using Tuple = std::tuple<Types...>;

    namespace TupleUtils
    {
        // -=-=-=-= Tuple Concat =-=-=-=-
        template <typename ATypes, typename BTypes, typename... RemainingTypes>
        struct Concat;

        template <typename... ATypes, typename... BTypes>
        struct Concat<Tuple<ATypes...>, Tuple<BTypes...>>
        {
            using Type = Tuple<ATypes..., BTypes...>;
        };

        template <typename... ATypes, typename... BTypes, typename... RemainingTypes>
        struct Concat<Tuple<ATypes...>, Tuple<BTypes...>, RemainingTypes...>
        {
            using Type = typename Concat<Tuple<ATypes..., BTypes...>, RemainingTypes...>::Type;
        };

        template <typename... Tuples>
        using Concat_T = typename Concat<Tuples...>::Type;

        // -=-=-=-= Contains Type =-=-=-=-
        template <typename T, typename... Types>
        struct ContainsType;

        template <typename T, typename... Types>
        struct ContainsType<T, Tuple<Types...>> : std::bool_constant<(std::is_same_v<T, Types> || ...)>
        {};

        template <typename T, typename... Types>
        constexpr inline bool ContainsType_V = ContainsType<T, Types...>::value;

        // -=-=-=-= Filter Duplicates =-=-=-=-
        template <typename RemainingTypes, typename FilteredTypes = Tuple<>>
        struct FilterDuplicates;

        template <bool IsDuplicate, typename T, typename RemainingTypes, typename FilteredTypes>
        struct FilterDuplicatesBranch;

        template <typename FilteredTypes>
        struct FilterDuplicates<Tuple<>, FilteredTypes>
        {
            using Type = FilteredTypes;
        };

        template <typename T, typename... RemainingTypes, typename... FilteredTypes>
        struct FilterDuplicates<Tuple<T, RemainingTypes...>, Tuple<FilteredTypes...>>
        {
            static constexpr bool IsDuplicate = ContainsType_V<T, Tuple<FilteredTypes...>>;
            using Type =
              typename FilterDuplicatesBranch<IsDuplicate, T, Tuple<RemainingTypes...>, Tuple<FilteredTypes...>>::Type;
        };

        template <typename T, typename... RemainingTypes, typename... FilteredTypes>
        struct FilterDuplicatesBranch<true, T, Tuple<RemainingTypes...>, Tuple<FilteredTypes...>>
        {
            using Type = typename FilterDuplicates<Tuple<RemainingTypes...>, Tuple<FilteredTypes...>>::Type;
        };

        template <typename T, typename... RemainingTypes, typename... FilteredTypes>
        struct FilterDuplicatesBranch<false, T, Tuple<RemainingTypes...>, Tuple<FilteredTypes...>>
        {
            using Type = typename FilterDuplicates<Tuple<RemainingTypes...>, Tuple<FilteredTypes..., T>>::Type;
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
        struct SortTypes<Comparator, Tuple<>>
        {
            using Type = Tuple<>;
        };

        template <template <typename, typename> class Comparator, typename T>
        struct SortTypes<Comparator, Tuple<T>>
        {
            using Type = Tuple<T>;
        };

        template <template <typename, typename> class Comparator, typename T, typename... RemainingTypes>
        struct SortTypes<Comparator, Tuple<T, RemainingTypes...>>
        {
            using Type = typename SortTypesImpl<Comparator, Tuple<RemainingTypes...>, T, Tuple<>, Tuple<>>::Type;
        };

        template <template <typename, typename> class Comparator,
                  typename Pivot,
                  typename... LHSTypes,
                  typename... RHSTypes>
        struct SortTypesImpl<Comparator, Tuple<>, Pivot, Tuple<LHSTypes...>, Tuple<RHSTypes...>>
        {
            using Type = typename Concat<typename SortTypes<Comparator, Tuple<LHSTypes...>>::Type,
                                         Tuple<Pivot>,
                                         typename SortTypes<Comparator, Tuple<RHSTypes...>>::Type>::Type;
        };

        template <template <typename, typename> class Comparator,
                  typename T,
                  typename... RemainingTypes,
                  typename Pivot,
                  typename... LHSTypes,
                  typename... RHSTypes>
        struct SortTypesImpl<Comparator, Tuple<T, RemainingTypes...>, Pivot, Tuple<LHSTypes...>, Tuple<RHSTypes...>>
        {
            using Type = typename SortTypesBranch<Comparator,
                                                  Comparator<T, Pivot>::value,
                                                  T,
                                                  Tuple<RemainingTypes...>,
                                                  Pivot,
                                                  Tuple<LHSTypes...>,
                                                  Tuple<RHSTypes...>>::Type;
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
                               Tuple<RemainingTypes...>,
                               Pivot,
                               Tuple<LHSTypes...>,
                               Tuple<RHSTypes...>>
        {
            using Type = typename SortTypesImpl<Comparator,
                                                Tuple<RemainingTypes...>,
                                                Pivot,
                                                Tuple<LHSTypes..., T>,
                                                Tuple<RHSTypes...>>::Type;
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
                               Tuple<RemainingTypes...>,
                               Pivot,
                               Tuple<LHSTypes...>,
                               Tuple<RHSTypes...>>
        {
            using Type = typename SortTypesImpl<Comparator,
                                                Tuple<RemainingTypes...>,
                                                Pivot,
                                                Tuple<LHSTypes...>,
                                                Tuple<RHSTypes..., T>>::Type;
        };

        template <template <typename, typename> class Comparator, typename Types>
        using SortTypes_T = typename SortTypes<Comparator, Types>::Type;

        // -=-=-=-= Sorting Comparators =-=-=-=-
        template <typename LHS, typename RHS>
        struct CompareLargerAlignment
        {
            static constexpr bool value = alignof(LHS) > alignof(RHS);
        };

    } // namespace TupleUtils
} // namespace Zen

#include "Zen_TupleUtils_Tests.inl"