#include <system_module/detail/posted_event_queue.hpp>
#include <system_module/system.hpp>

#include <gtest/gtest.h>

#include <memory>

TEST(PostedEventQueueTest, AddEvent)
{
	mcurses::System system;
	mcurses::detail::Posted_event_queue queue;
	EXPECT_TRUE(queue.empty());

	mcurses::Object object1;
	auto event1 = std::make_unique<mcurses::Event>(mcurses::Event::Type::None);	// change once abstract
	mcurses::detail::Posted_event pe1(&object1, std::move(event1), 0);
	queue.add_event(pe1);

	mcurses::Object object2;
	auto event2 = std::make_unique<mcurses::Event>(mcurses::Event::Type::None);	// change once abstract
	mcurses::detail::Posted_event pe2(&object2, std::move(event2), 0);
	queue.add_event(pe2);

	mcurses::Object object3;
	auto event3 = std::make_unique<mcurses::Event>(mcurses::Event::Type::None);	// change once abstract
	mcurses::detail::Posted_event pe3(&object3, std::move(event3), 1);
	queue.add_event(pe3);

	mcurses::Object object4;
	auto event4 = std::make_unique<mcurses::Event>(mcurses::Event::Type::None);	// change once abstract
	mcurses::detail::Posted_event pe4(&object4, std::move(event4), -9);
	queue.add_event(pe4);

	mcurses::Object object5;
	auto event5 = std::make_unique<mcurses::Event>(mcurses::Event::Type::None);	// change once abstract
	mcurses::detail::Posted_event pe5(&object5, std::move(event5), 100);
	queue.add_event(pe5);

	mcurses::Object object6;
	auto event6 = std::make_unique<mcurses::Event>(mcurses::Event::Type::None);	// change once abstract
	mcurses::detail::Posted_event pe6(&object6, std::move(event6), 0);
	queue.add_event(pe6);

	EXPECT_EQ(queue.next_posted_event().priority(), 100);
	EXPECT_EQ(queue.next_posted_event().priority(), 1);
	EXPECT_EQ(queue.next_posted_event().priority(), 0);
	EXPECT_EQ(queue.next_posted_event().priority(), 0);
	EXPECT_EQ(queue.next_posted_event().priority(), 0);
	EXPECT_EQ(queue.next_posted_event().priority(), -9);
}

TEST(PostedEventQueueTest, NextEvent)
{
	mcurses::System system;
	mcurses::detail::Posted_event_queue queue;

	mcurses::Object object1;
	auto event1 = std::make_unique<mcurses::Event>(mcurses::Event::Type::None);	// change once abstract
	mcurses::detail::Posted_event pe1(&object1, std::move(event1), 0);
	queue.add_event(pe1);

	mcurses::Object object2;
	auto event2 = std::make_unique<mcurses::Event>(mcurses::Event::Type::None);	// change once abstract
	mcurses::detail::Posted_event pe2(&object2, std::move(event2), 0);
	queue.add_event(pe2);

	EXPECT_EQ(2, queue.size());
	queue.next_posted_event();
	EXPECT_EQ(1, queue.size());
	queue.next_posted_event();
	EXPECT_EQ(0, queue.size());
}
