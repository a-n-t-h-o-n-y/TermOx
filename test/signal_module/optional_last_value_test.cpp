#include <mcurses/signal_module/optional.hpp>
#include <mcurses/signal_module/optional_last_value.hpp>
#include <gtest/gtest.h>
#include <vector>

TEST(OptionalLastValueTest, VectorTest)
{
	std::vector<int> vec{1,2,3,4,5};
	mcurses::optional_last_value<int> olv{};
	mcurses::optional_last_value<int>::result_type result;
	result = olv(std::begin(vec), std::end(vec));
	ASSERT_TRUE(bool(result));
	EXPECT_EQ(5,*result);
}

TEST(OptionalLastValueTest, EmptyRange)
{
	std::vector<int> vec{};
	mcurses::optional_last_value<int> olv{};
	mcurses::optional_last_value<int>::result_type result;
	result = olv(std::begin(vec), std::end(vec));
	EXPECT_FALSE(bool(result));
	EXPECT_EQ(nullptr, result.get_ptr());
}
