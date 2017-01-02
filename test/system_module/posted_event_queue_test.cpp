#include <system_module/detail/posted_event_queue.hpp>
#include <system_module/system.hpp>

#include <gtest/gtest.h>

#include <memory>

using twf::detail::Posted_event;
using twf::detail::Posted_event_queue;
using twf::Event;
using twf::Object;
using twf::System;

TEST(PostedEventQueueTest, AddEvent) {
    System system;
    Posted_event_queue queue;
    EXPECT_TRUE(queue.empty());

    Object object1;
    auto event1 = std::make_unique<Event>(Event::None);  // change once abstract
    Posted_event pe1{&object1, std::move(event1), 0};
    queue.add_event(std::move(pe1));

    Object object2;
    auto event2 = std::make_unique<Event>(Event::None);  // change once abstract
    Posted_event pe2{&object2, std::move(event2), 0};
    queue.add_event(std::move(pe2));

    Object object3;
    auto event3 = std::make_unique<Event>(Event::None);  // change once abstract
    Posted_event pe3{&object3, std::move(event3), 1};
    queue.add_event(std::move(pe3));

    Object object4;
    auto event4 = std::make_unique<Event>(Event::None);  // change once abstract
    Posted_event pe4{&object4, std::move(event4), -9};
    queue.add_event(std::move(pe4));

    Object object5;
    auto event5 = std::make_unique<Event>(Event::None);  // change once abstract
    Posted_event pe5{&object5, std::move(event5), 100};
    queue.add_event(std::move(pe5));

    Object object6;
    auto event6 = std::make_unique<Event>(Event::None);  // change once abstract
    Posted_event pe6{&object6, std::move(event6), 0};
    queue.add_event(std::move(pe6));

    EXPECT_EQ(std::begin(queue)->priority(), 100);
    queue.erase(std::begin(queue));
    EXPECT_EQ(std::begin(queue)->priority(), 1);
    queue.erase(std::begin(queue));
    EXPECT_EQ(std::begin(queue)->priority(), 0);
    queue.erase(std::begin(queue));
    EXPECT_EQ(std::begin(queue)->priority(), 0);
    queue.erase(std::begin(queue));
    EXPECT_EQ(std::begin(queue)->priority(), 0);
    queue.erase(std::begin(queue));
    EXPECT_EQ(std::begin(queue)->priority(), -9);
    queue.erase(std::begin(queue));
}

TEST(PostedEventQueueTest, NextEvent) {
    System system;
    Posted_event_queue queue;

    Object object1;
    auto event1 = std::make_unique<Event>(Event::None);  // change once abstract
    Posted_event pe1{&object1, std::move(event1), 0};
    queue.add_event(std::move(pe1));

    Object object2;
    auto event2 = std::make_unique<Event>(Event::None);  // change once abstract
    Posted_event pe2{&object2, std::move(event2), 0};
    queue.add_event(std::move(pe2));

    EXPECT_EQ(2, queue.size());
    queue.erase(std::begin(queue));
    EXPECT_EQ(1, queue.size());
    queue.erase(std::begin(queue));
    EXPECT_EQ(0, queue.size());
}
