// Should this be able to be disabled?
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