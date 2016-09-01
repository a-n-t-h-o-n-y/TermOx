#include "signal_module/none.hpp"
#include <gtest/gtest.h>

TEST(NoneTest, BoolConversion)
{
	EXPECT_FALSE(mcurses::none);
}