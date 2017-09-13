#include "signals/connection.hpp"
#include "signals/detail/connection_impl.hpp"
#include "signals/slot.hpp"
#include <gtest/gtest.h>
#include <memory>

using sig::Connection;
using sig::Connection_impl;
using sig::Slot;

TEST(ConnectionImplTest, DefaultConstructor) {
    Connection_impl<void(int)> impl;
    EXPECT_FALSE(impl.connected());
    EXPECT_FALSE(impl.blocked());
}

TEST(ConnectionImplTest, SlotConstructor) {
    Slot<void(int)> s = [](int) { return; };
    Connection_impl<void(int)> impl(s);
    EXPECT_TRUE(impl.connected());
    EXPECT_FALSE(impl.blocked());
}

TEST(ConnectionImplTest, DisconnectMethod) {
    Slot<void(int)> s = [](int) { return; };
    Connection_impl<void(int)> impl(s);
    EXPECT_TRUE(impl.connected());
    EXPECT_FALSE(impl.blocked());

    impl.disconnect();
    EXPECT_FALSE(impl.connected());
    EXPECT_FALSE(impl.blocked());
}

TEST(ConnectionImplTest, BlockedMethod) {
    Connection_impl<void(int)> impl1;
    EXPECT_FALSE(impl1.blocked());

    Slot<void(int)> s = [](int) { return; };
    Connection_impl<void(int)> impl2(s);
    EXPECT_FALSE(impl2.blocked());
}

TEST(ConnectionImplTest, ConnectedMethod) {
    Slot<void(int)> s = [](int) { return; };
    Connection_impl<void(int)> impl1(s);
    EXPECT_TRUE(impl1.connected());

    Connection_impl<void(int)> impl2;
    EXPECT_FALSE(impl2.connected());
}

TEST(ConnectionImplTest, GetSlotMethod) {
    Slot<int(int)> s1 = [](int) { return 5; };
    Connection_impl<int(int)> impl(s1);

    EXPECT_EQ(5, (impl.get_slot())(1));

    Slot<int(int)> s2 = [](int) { return 7; };
    impl.get_slot() = s2;

    EXPECT_EQ(7, (impl.get_slot())(1));
}

TEST(ConnectionImplTest, ConstGetSlotMethod) {
    Slot<int(int)> s1 = [](int) { return 5; };
    const Connection_impl<int(int)> impl(s1);

    EXPECT_EQ(5, (impl.get_slot())(1));
}

TEST(ConnectionImplTest, EmplaceExtended) {
    auto ci = std::make_shared<Connection_impl<int(double)>>();
    Slot<int(const Connection&, double)> es = [](const Connection&, double) {
        return 3;
    };
    Connection conn{ci};
    ci->emplace_extended(es, conn);
    EXPECT_TRUE(conn.connected());
    EXPECT_EQ(3, ci->get_slot()(5.4));
}
