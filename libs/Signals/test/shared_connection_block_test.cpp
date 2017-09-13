#include "signals/connection.hpp"
#include "signals/shared_connection_block.hpp"
#include "signals/signal.hpp"
#include "signals/slot.hpp"
#include <gtest/gtest.h>

using sig::Shared_connection_block;
using sig::Signal;
using sig::Slot;
using sig::Connection;

TEST(SharedConnectionBlockTest, DefaultConstructor) {
    Shared_connection_block scb1{};
    EXPECT_FALSE(scb1.blocking());

    Signal<int(char)> sig1;
    Slot<int(char)> slot_1 = [](char) { return 3; };
    auto conn = sig1.connect(slot_1);

    EXPECT_FALSE(conn.blocked());

    Shared_connection_block scb2(conn);

    EXPECT_TRUE(conn.blocked());
    EXPECT_TRUE(scb2.blocking());
    EXPECT_FALSE(bool(sig1('g')));

    Signal<int(char)> sig2;
    Slot<int(char)> slot_2 = [](char) { return 3; };
    auto conn2 = sig2.connect(slot_2);

    EXPECT_FALSE(conn2.blocked());

    Shared_connection_block scb3(conn2, false);
    EXPECT_FALSE(conn2.blocked());
    EXPECT_FALSE(scb3.blocking());

    scb3.block();
    EXPECT_TRUE(conn2.blocked());
    EXPECT_TRUE(scb3.blocking());
}

TEST(SharedConnectionBlockTest, CopyConstructor) {
    Signal<int(char)> sig1;
    Slot<int(char)> slot_1 = [](char) { return 3; };
    auto conn1 = sig1.connect(slot_1);
    Shared_connection_block scb1(conn1);

    EXPECT_TRUE(scb1.blocking());

    Shared_connection_block scb1_copy(scb1);

    EXPECT_TRUE(scb1_copy.blocking());
    EXPECT_TRUE(conn1.blocked());

    scb1.unblock();
    EXPECT_FALSE(scb1.blocking());
    EXPECT_TRUE(scb1_copy.blocking());
    EXPECT_TRUE(conn1.blocked());

    scb1_copy.block();
    EXPECT_TRUE(scb1_copy.blocking());
    EXPECT_FALSE(scb1.blocking());
    EXPECT_TRUE(conn1.blocked());

    scb1_copy.unblock();
    EXPECT_FALSE(scb1.blocking());
    EXPECT_FALSE(conn1.blocked());
}

TEST(SharedConnectionBlockTest, CopyAssignmentOperator) {
    Signal<int()> sig1;
    Slot<int()> slot_1 = []() { return 4; };
    auto conn1 = sig1.connect(slot_1);

    Shared_connection_block scb1{conn1};
    Shared_connection_block scb2;
    scb2 = scb1;

    EXPECT_TRUE(conn1.blocked());
    EXPECT_TRUE(scb2.blocking());

    scb1.unblock();

    EXPECT_TRUE(scb2.blocking());
    EXPECT_FALSE(scb1.blocking());
    EXPECT_TRUE(conn1.blocked());

    scb2.unblock();

    EXPECT_FALSE(scb1.blocking());
    EXPECT_FALSE(scb2.blocking());
    EXPECT_FALSE(conn1.blocked());

    scb1.block();

    EXPECT_TRUE(scb1.blocking());
    EXPECT_FALSE(scb2.blocking());
    EXPECT_TRUE(conn1.blocked());

    Signal<int(double)> sig2;
    Slot<int(double)> slot_2 = [](double) { return 4; };
    auto conn2 = sig2.connect(slot_2);
    Slot<int(const Connection&, double)> slot_ext = [](const Connection&,
                                                       double) { return 5; };
    auto conn3 = sig2.connect_extended(1, slot_ext);

    Shared_connection_block scb4{conn2};
    Shared_connection_block scb3{conn3};

    EXPECT_TRUE(conn2.blocked());
    EXPECT_TRUE(conn3.blocked());
    EXPECT_FALSE(bool(sig2(5.4)));

    scb3 = scb4;

    EXPECT_TRUE(conn2.blocked());
    EXPECT_FALSE(conn3.blocked());
    EXPECT_TRUE(scb4.blocking());
    EXPECT_TRUE(scb3.blocking());
    ASSERT_TRUE(bool(sig2(5.4)));
    EXPECT_EQ(5, *sig2(5.4));

    scb4.unblock();
    EXPECT_TRUE(conn2.blocked());
    EXPECT_FALSE(conn3.blocked());
    EXPECT_FALSE(scb4.blocking());
    EXPECT_TRUE(scb3.blocking());
    ASSERT_TRUE(bool(sig2(5.4)));
    EXPECT_EQ(5, *sig2(5.4));

    scb3.unblock();
    EXPECT_FALSE(conn2.blocked());
    EXPECT_FALSE(conn3.blocked());
    EXPECT_FALSE(scb4.blocking());
    EXPECT_FALSE(scb3.blocking());
    ASSERT_TRUE(bool(sig2(5.4)));
    EXPECT_EQ(4, *sig2(5.4));
}

TEST(SharedConnectionBlockTest, Destructor) {
    Signal<int(double)> sig1;
    Slot<int(double)> slot_1;
    auto conn1 = sig1.connect(slot_1);

    auto scb_ptr = std::make_shared<Shared_connection_block>(conn1);

    EXPECT_TRUE(scb_ptr->blocking());
    EXPECT_TRUE(conn1.blocked());

    scb_ptr.reset();

    EXPECT_FALSE(conn1.blocked());
}

TEST(SharedConnectionBlockTest, Block) {
    Signal<int()> sig;
    Slot<int()> slot_1;
    auto conn1 = sig.connect(slot_1);

    Shared_connection_block scb{conn1, false};
    EXPECT_FALSE(conn1.blocked());
    EXPECT_FALSE(scb.blocking());

    scb.block();
    EXPECT_TRUE(conn1.blocked());
    EXPECT_TRUE(scb.blocking());

    scb.block();
    EXPECT_TRUE(conn1.blocked());
    EXPECT_TRUE(scb.blocking());

    scb.block();
    scb.block();
    scb.block();
    EXPECT_TRUE(conn1.blocked());
    EXPECT_TRUE(scb.blocking());

    scb.unblock();
    EXPECT_FALSE(conn1.blocked());
    EXPECT_FALSE(scb.blocking());

    Shared_connection_block scb2{conn1};
    scb.block();
    EXPECT_TRUE(conn1.blocked());
    EXPECT_TRUE(scb.blocking());
    EXPECT_TRUE(scb2.blocking());
    scb.block();
    scb2.block();
    EXPECT_TRUE(conn1.blocked());
    EXPECT_TRUE(scb.blocking());
    EXPECT_TRUE(scb2.blocking());
    scb.unblock();
    EXPECT_TRUE(conn1.blocked());
    EXPECT_FALSE(scb.blocking());
    EXPECT_TRUE(scb2.blocking());
    scb2.unblock();
    EXPECT_FALSE(conn1.blocked());
    EXPECT_FALSE(scb.blocking());
    EXPECT_FALSE(scb2.blocking());
}

TEST(SharedConnectionBlockTest, UnBlock) {
    Signal<void(int)> sig;
    Slot<void(int)> slot_1 = [](int) { return; };
    auto conn1 = sig.connect(slot_1);

    Shared_connection_block scb{conn1, true};
    EXPECT_TRUE(conn1.blocked());
    EXPECT_TRUE(scb.blocking());

    scb.unblock();
    scb.unblock();
    scb.unblock();
    scb.unblock();
    scb.unblock();
    scb.unblock();

    EXPECT_FALSE(conn1.blocked());
    EXPECT_FALSE(scb.blocking());
}

TEST(SharedConnectionBlockTest, Blocking) {
    Shared_connection_block scb1;
    EXPECT_FALSE(scb1.blocking());

    Signal<int()> sig;
    Slot<int()> slot_1 = []() { return 2; };
    auto conn1 = sig.connect(slot_1);

    Shared_connection_block scb2{conn1};
    Shared_connection_block scb3{conn1, false};

    EXPECT_TRUE(scb2.blocking());
    EXPECT_FALSE(scb3.blocking());

    scb3.block();
    EXPECT_TRUE(scb3.blocking());

    scb2.unblock();
    EXPECT_FALSE(scb2.blocking());
}

TEST(SharedConnectionBlockTest, Connection) {
    Signal<int()> sig;
    Slot<int()> slot_1 = []() { return 2; };
    auto conn1 = sig.connect(slot_1);

    Shared_connection_block scb2{conn1};

    EXPECT_TRUE(conn1.connected());
    scb2.connection().disconnect();
    EXPECT_FALSE(conn1.connected());

    Shared_connection_block scb_empty;
    EXPECT_FALSE(scb_empty.connection().connected());
}
