#include <mcurses/signal_module/detail/connection_impl.hpp>
#include <mcurses/signal_module/slot.hpp>
#include <gtest/gtest.h>

TEST(ConnectionImplTest, DefaultConstructor)
{
	mcurses::connection_impl<void(int)> impl;
	EXPECT_FALSE(impl.connected());
	EXPECT_FALSE(impl.blocked());
}

TEST(ConnectionImplTest, SlotConstructor)
{
	mcurses::slot<void(int)> s = [](int){return;};
	mcurses::connection_impl<void(int)> impl(s);
	EXPECT_TRUE(impl.connected());
	EXPECT_FALSE(impl.blocked());
}

TEST(ConnectionImplTest, DisconnectMethod)
{
	mcurses::slot<void(int)> s = [](int){return;};
	mcurses::connection_impl<void(int)> impl(s);
	EXPECT_TRUE(impl.connected());
	EXPECT_FALSE(impl.blocked());

	impl.disconnect();
	EXPECT_FALSE(impl.connected());
	EXPECT_FALSE(impl.blocked());
}

TEST(ConnectionImplTest, BlockedMethod)
{
	mcurses::connection_impl<void(int)> impl1;
	EXPECT_FALSE(impl1.blocked());
	
	mcurses::slot<void(int)> s = [](int){return;};
	mcurses::connection_impl<void(int)> impl2(s);
	EXPECT_FALSE(impl2.blocked());

	// Fill in once you have shared_connection_block
}

TEST(ConnectionImplTest, ConnectedMethod)
{
	mcurses::slot<void(int)> s = [](int){return;};
	mcurses::connection_impl<void(int)> impl1(s);
	EXPECT_TRUE(impl1.connected());

	mcurses::connection_impl<void(int)> impl2;
	EXPECT_FALSE(impl2.connected());
}

TEST(ConnectionImplTest, GetSlotMethod)
{
	mcurses::slot<int(int)> s1 = [](int){return 5;};
	mcurses::connection_impl<int(int)> impl(s1);

	EXPECT_EQ(5, (impl.get_slot())(1));

	mcurses::slot<int(int)> s2 = [](int){return 7;};
	impl.get_slot() = s2;

	EXPECT_EQ(7, (impl.get_slot())(1));
}

TEST(ConnectionImplTest, ConstGetSlotMethod)
{
	mcurses::slot<int(int)> s1 = [](int){return 5;};
	const mcurses::connection_impl<int(int)> impl(s1);

	EXPECT_EQ(5, (impl.get_slot())(1));
}