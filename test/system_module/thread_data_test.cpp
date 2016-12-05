#include <system_module/detail/thread_data.hpp>
#include <system_module/system.hpp>

#include <gtest/gtest.h>

TEST(ThreadDataTest, Default)
{
	mcurses::System system;
	auto& data = mcurses::detail::Thread_data::current();
	EXPECT_TRUE(data.can_wait);
	EXPECT_FALSE(data.quit_now);
	EXPECT_EQ(0, data.loop_level);
	mcurses::Event_loop el;
}
