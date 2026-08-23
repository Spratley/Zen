#pragma once

#include "../Zen_Types.h"
#include "Zen_LoopUtils.h"
#include "Zen_TypeUtils.h"

#include <type_traits>

namespace Zen
{
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

        // -=-=-=-= Get First =-=-=-=-
        template <typename Types>
        struct GetFirst;

        template <typename FirstType, typename... RemainingTypes>
        struct GetFirst<TypeList<FirstType, RemainingTypes...>>
        {
            using Type = FirstType;
        };

        template <typename Types>
        using GetFirst_T = typename GetFirst<Types>::Type;

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
        struct IndexOf<T, TypeList<Types...>>
        {
            static consteval SizeT FindIndex()
            {
                /*SizeT index = 0;
                bool found = ((std::is_same_v<T, Types> ? true : (++index, false)) || ...);
                return found ? index : SizeT_Max;*/
                // ^ I had this whole cool fold expression here that abused short circuit evaluation of ||,
                // then I realized that it's consteval and an array + loop is much more readable.
                // Keeping it around as a comment anyway for the memories... :)

                constexpr SizeT typeCount = sizeof...(Types);
                if constexpr (typeCount == 0)
                {
                    return SizeT_Max;
                }

                constexpr bool matchesType[] = { std::is_same_v<T, Types>... };
                for (auto index : LoopUtils::CountTo(typeCount))
                {
                    if (matchesType[index])
                    {
                        return index;
                    }
                }
                return SizeT_Max;
            }
            static constexpr SizeT value = FindIndex();
        };

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
              typename FilterDuplicatesBranch<IsDuplicate, T, TypeList<RemainingTypes...>, TypeList<FilteredTypes...>>::
                Type;
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
            using Type =
              typename SortTypesImpl<Comparator, TypeList<RemainingTypes...>, T, TypeList<>, TypeList<>>::Type;
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
        struct SortTypesImpl<Comparator,
                             TypeList<T, RemainingTypes...>,
                             Pivot,
                             TypeList<LHSTypes...>,
                             TypeList<RHSTypes...>>
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
        struct CompareECSPacking
        {
            static consteval bool Compare()
            {
                // Return the bigger alignemnt
                if constexpr (alignof(LHS) != alignof(RHS))
                {
                    return alignof(LHS) > alignof(RHS);
                }
                // If alignment is the same, return the bigger size
                if constexpr (sizeof(LHS) != sizeof(RHS))
                {
                    return sizeof(LHS) > sizeof(RHS);
                }
                // Just order by arbitrary hash if alignment and size are identical
                return TypeUtils::HashType_V<LHS, U64> < TypeUtils::HashType_V<RHS, U64>;
            }
            static constexpr bool value = Compare();
        };
    } // namespace TypeListUtils
} // namespace Zen

#if defined(ZEN_ENABLE_UNIT_TEST)
namespace Zen
{
    namespace TypeListUtils_Tests
    {
        template <int Value>
        struct TestType
        {
            static constexpr int s_value = Value;
        };

        using A = TestType<1>;
        using B = TestType<2>;
        using C = TestType<3>;
        using D = TestType<4>;

        template <typename LHS, typename RHS>
        struct CompareTestValue
        {
            static constexpr bool value = LHS::s_value < RHS::s_value;
        };

        static_assert((A::s_value < B::s_value) && (B::s_value < C::s_value) && (C::s_value < D::s_value),
                      "Test type values must be in ascending order! Tests will erroneously fail otherwise!");

        namespace TestConcat
        {
            using Input = TypeList<A, B>;
            using Expected = TypeList<A, B, A, B>;
            static_assert(std::is_same_v<TypeListUtils::Concat_T<Input, Input>, Expected>, "TypeList Concat Failed!");
        } // namespace TestConcat

        namespace TestGetFirst
        {
            using Input = TypeList<A, B, C>;
            using Expected = A;
            static_assert(std::is_same_v<TypeListUtils::GetFirst_T<Input>, Expected>, "Get First Failed!");
        } // namespace TestGetFirst

        namespace TestContainsType
        {
            using Input = TypeList<A, B, C>;
            static_assert(TypeListUtils::ContainsType_V<B, Input>, "TypeList Contains Type Failed to Detect Type!");
            static_assert(!TypeListUtils::ContainsType_V<D, Input>,
                          "TypeList Contains Type Failed to Detect Missing Type!");
        } // namespace TestContainsType

        namespace TestIndexOf
        {
            using Input = TypeList<A, B, C>;
            static_assert(TypeListUtils::IndexOf_V<B, Input> == 1, "TypeList Index Of Failed!");
        } // namespace TestIndexOf

        namespace TestFilterDuplicates
        {
            using Input = TypeList<A, B, B, A, C, B, A>;
            using Expected = TypeList<A, B, C>;
            static_assert(std::is_same_v<TypeListUtils::FilterDuplicates_T<Input>, Expected>,
                          "TypeList Filter Duplicates Failed!");
        } // namespace TestFilterDuplicates

        namespace TestSortTypes
        {
            using Input = TypeList<C, B, D, A>;
            using Expected = TypeList<A, B, C, D>;
            static_assert(std::is_same_v<TypeListUtils::SortTypes_T<CompareTestValue, Input>, Expected>,
                          "TypeList Sort Types Failed!");
        } // namespace TestSortTypes
    } // namespace TypeListUtils_Tests
} // namespace Zen
#endif // defined(ZEN_ENABLE_UNIT_TEST)