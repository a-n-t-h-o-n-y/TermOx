#include "signals/optional_last_value.hpp"
#include <gtest/gtest.h>
#include <vector>

using sig::Optional_last_value;

TEST(OptionalLastValueTest, VectorTest) {
    std::vector<int> vec{1, 2, 3, 4, 5};
    Optional_last_value<int> olv{};
    Optional_last_value<int>::result_type result;
    result = olv(std::begin(vec), std::end(vec));
    ASSERT_TRUE(bool(result));
    EXPECT_EQ(5, *result);
}

TEST(OptionalLastValueTest, EmptyRange) {
    std::vector<int> vec{};
    Optional_last_value<int> olv{};
    Optional_last_value<int>::result_type result;
    result = olv(std::begin(vec), std::end(vec));
    EXPECT_FALSE(bool(result));
}
