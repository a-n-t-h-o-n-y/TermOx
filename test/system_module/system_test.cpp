#include <mcurses/system_module/system.hpp>
#include <mcurses/system_module/event.hpp>
#include <mcurses/system_module/object.hpp>
#include <mcurses/system_module/detail/thread_data.hpp>
#include <mcurses/system_module/detail/posted_event.hpp>
#include <mcurses/system_module/detail/posted_event_queue.hpp>
#include <gtest/gtest.h>

#include <memory>

TEST(SystemTest, DefaultConstructor)
{
	// mcurses::System system;
	// EXPECT_EQ(0, system.run());
}

TEST(SystemTest, PostEvent)
{
	// mcurses::Object obj;
	// mcurses::System::post_event(&obj, std::make_unique<mcurses::Event>(mcurses::Event::Type::None));
	// ^ This screws up the next test because of global objects.
}

TEST(SystemTest, RemovePostedEvent)
{
	mcurses::detail::Posted_event_queue& queue = mcurses::detail::Thread_data::current().event_queue;
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
	auto ptr3 = event3.get();
	mcurses::detail::Posted_event pe3(&object3, std::move(event3), 1);
	queue.add_event(pe3);

	mcurses::Object object4;
	auto event4 = std::make_unique<mcurses::Event>(mcurses::Event::Type::None);	// change once abstract
	auto ptr4 = event4.get();
	mcurses::detail::Posted_event pe4(&object4, std::move(event4), -9);
	queue.add_event(pe4);

	mcurses::Object object5;
	auto event5 = std::make_unique<mcurses::Event>(mcurses::Event::Type::None);	// change once abstract
	auto ptr5 = event5.get();
	mcurses::detail::Posted_event pe5(&object5, std::move(event5), 100);
	queue.add_event(pe5);

	mcurses::Object object6;
	auto event6 = std::make_unique<mcurses::Event>(mcurses::Event::Type::None);	// change once abstract
	auto ptr6 = event6.get();
	mcurses::detail::Posted_event pe6(&object6, std::move(event6), 0);
	queue.add_event(pe6);

	EXPECT_EQ(6, queue.size());
	mcurses::System::remove_posted_event(nullptr);
	EXPECT_EQ(6, queue.size());

	mcurses::System::remove_posted_event(ptr3);
	EXPECT_EQ(5, queue.size());
	mcurses::System::remove_posted_event(ptr4);
	EXPECT_EQ(4, queue.size());
	mcurses::System::remove_posted_event(ptr5);
	EXPECT_EQ(3, queue.size());
	mcurses::System::remove_posted_event(ptr6);
	EXPECT_EQ(2, queue.size());

}