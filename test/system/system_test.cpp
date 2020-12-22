#include <system/detail/posted_event.hpp>
#include <system/detail/posted_event_queue.hpp>
#include <system/detail/thread_data.hpp>
#include <system/event.hpp>
#include <system/object.hpp>
#include <system/system.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <utility>

using ox::Event;
using ox::Object;
using ox::System;
using ox::detail::Posted_event;
using ox::detail::Posted_event_queue;
using ox::detail::Thread_data;

TEST(SystemTest, DefaultConstructor) { System system; }

TEST(SystemTest, PostEvent)
{
    System system;
    Object obj;
    auto event     = std::make_unique<Event>(Event::None);
    auto event_ptr = event.get();
    System::post_event(&obj, std::move(event));
    System::remove_posted_event(event_ptr);
}

TEST(SystemTest, RemovePostedEvent)
{
    System system;
    Posted_event_queue& queue = Thread_data::current().event_queue;
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
    auto ptr3   = event3.get();
    Posted_event pe3{&object3, std::move(event3), 1};
    queue.add_event(std::move(pe3));

    Object object4;
    auto event4 = std::make_unique<Event>(Event::None);  // change once abstract
    auto ptr4   = event4.get();
    Posted_event pe4{&object4, std::move(event4), -9};
    queue.add_event(std::move(pe4));

    Object object5;
    auto event5 = std::make_unique<Event>(Event::None);  // change once abstract
    auto ptr5   = event5.get();
    Posted_event pe5{&object5, std::move(event5), 100};
    queue.add_event(std::move(pe5));

    Object object6;
    auto event6 = std::make_unique<Event>(Event::None);  // change once abstract
    auto ptr6   = event6.get();
    Posted_event pe6{&object6, std::move(event6), 0};
    queue.add_event(std::move(pe6));

    EXPECT_EQ(6, queue.size());
    System::remove_posted_event(nullptr);
    EXPECT_EQ(6, queue.size());

    System::remove_posted_event(ptr3);
    EXPECT_EQ(5, queue.size());
    System::remove_posted_event(ptr4);
    EXPECT_EQ(4, queue.size());
    System::remove_posted_event(ptr5);
    EXPECT_EQ(3, queue.size());
    System::remove_posted_event(ptr6);
    EXPECT_EQ(2, queue.size());
}
