#include <system/detail/thread_data.hpp>
#include <system/event_loop.hpp>
#include <system/system.hpp>

#include <gtest/gtest.h>

using ox::Event_loop;
using ox::System;
using ox::detail::Thread_data;

TEST(ThreadDataTest, Default)
{
    System system;
    auto& data = Thread_data::current();
    EXPECT_TRUE(data.can_wait);
    EXPECT_FALSE(data.quit_now);
    EXPECT_EQ(0, data.loop_level);
    Event_loop el;
}
