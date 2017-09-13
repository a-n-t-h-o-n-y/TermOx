#include "signals/slot.hpp"
#include "signals/slot_base.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <vector>

using sig::Slot;

TEST(SlotBaseTest, Expired) {
    Slot<void(int)> s = {[](int) { return; }};
    EXPECT_FALSE(s.expired());

    auto s_char = std::make_shared<char>('h');
    s.track(s_char);
    EXPECT_FALSE(s.expired());

    s.track(std::make_shared<int>(5));
    EXPECT_TRUE(s.expired());

    Slot<void(int)> s2 = {[](int) { return; }};
    auto s_int = std::make_shared<int>(7);
    auto s_char2 = std::make_shared<char>('y');
    s2.track(s_int);
    s2.track(s_char2);
    EXPECT_FALSE(s2.expired());
    s_int.reset();
    EXPECT_TRUE(s2.expired());

    auto s_dbl = std::make_shared<double>(2.5);
    s2.track(s_dbl);
    EXPECT_TRUE(s2.expired());
}

// Add Tracked items and check again
TEST(SlotBaseTest, Lock) {
    Slot<void(int)> s = {[](int) { return; }};
    EXPECT_EQ(std::vector<std::shared_ptr<void>>{}, s.lock());

    auto s_char = std::make_shared<char>('h');
    s.track(s_char);

    EXPECT_EQ(1, s.lock().size());

    s.track(std::make_shared<double>(5));

    EXPECT_EQ(2, s.lock().size());

    EXPECT_TRUE(s.expired());
}
