#include <type_traits>

// Should this be able to be disabled?
namespace Zen
{
    namespace TupleUtils_Tests
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
            using Input = Tuple<A, B>;
            using Expected = Tuple<A, B, A, B>;
            static_assert(std::is_same_v<TupleUtils::Concat_T<Input, Input>, Expected>, "Tuple Concat Failed!");
        } // namespace TestConcat

        namespace TestContainsType
        {
            using Input = Tuple<A, B, C>;
            static_assert(TupleUtils::ContainsType_V<B, Input>, "Tuple Contains Type Failed to Detect Type!");
            static_assert(!TupleUtils::ContainsType_V<D, Input>, "Tuple Contains Type Failed to Detect Missing Type!");
        } // namespace TestContainsType

        namespace TestFilterDuplicates
        {
            using Input = Tuple<A, B, B, A, C, B, A>;
            using Expected = Tuple<A, B, C>;
            static_assert(std::is_same_v<TupleUtils::FilterDuplicates_T<Input>, Expected>,
                          "Tuple Filter Duplicates Failed!");
        } // namespace TestFilterDuplicates

        namespace TestSortTypes
        {
            using Input = Tuple<C, B, D, A>;
            using Expected = Tuple<A, B, C, D>;
            static_assert(std::is_same_v<TupleUtils::SortTypes_T<CompareTestValue, Input>, Expected>,
                          "Tuple Sort Types Failed!");
        } // namespace TestSortTypes
    } // namespace TupleUtils_Tests
} // namespace Zen