#include <system_module/detail/thread_data.hpp>
#include <system_module/event_loop.hpp>
#include <system_module/system.hpp>

#include <gtest/gtest.h>

using mcurses::System;
using mcurses::Event_loop;
using mcurses::detail::Thread_data;

TEST(ThreadDataTest, Default) {
    System system;
    auto& data = Thread_data::current();
    EXPECT_TRUE(data.can_wait);
    EXPECT_FALSE(data.quit_now);
    EXPECT_EQ(0, data.loop_level);
    Event_loop el;
}
