#include <system_module/detail/posted_event.hpp>
#include <system_module/event.hpp>
#include <system_module/object.hpp>
#include <system_module/system.hpp>

#include <gtest/gtest.h>

using cppurses::System;
using cppurses::Object;
using cppurses::detail::Posted_event;
using cppurses::Event;

TEST(PostedEventTest, LessThanOperator) {
    System system;
    Object obj;
    auto ev = std::make_unique<Event>(Event::None);
    //^^ change this when Event becomes pure virtual
    Posted_event pe1(&obj, std::move(ev), 1);
    Posted_event pe4(&obj, std::make_unique<Event>(Event::None), 4);
    // change this too ^^
    Posted_event pe0(&obj, std::make_unique<Event>(Event::None), 0);

    EXPECT_TRUE(pe1 < 4);
    EXPECT_TRUE(pe0 < 4);
    EXPECT_TRUE(pe0 < 1);
    EXPECT_TRUE(pe4 < 100);
    EXPECT_FALSE(pe4 < 4);
    EXPECT_FALSE(pe0 < 0);
    EXPECT_FALSE(pe0 < -10);
    EXPECT_TRUE(3 < pe4);
    EXPECT_FALSE(100 < pe1);
    EXPECT_TRUE(-5 < pe0);
}
