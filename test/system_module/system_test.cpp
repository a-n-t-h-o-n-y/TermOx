#include <mcurses/system_module/system.hpp>
#include <gtest/gtest.h>

TEST(SystemTest, DefaultConstructor)
{
	mcurses::System system;
	EXPECT_EQ(0, system.run());
}