#include <mcurses/system_module/detail/posted_event.hpp>
#include <mcurses/system_module/object.hpp>
#include <mcurses/system_module/event.hpp>
#include <gtest/gtest.h>

TEST(PostedEventTest, LessThanOperator)
{
	mcurses::Object obj;
	auto ev = std::make_unique<mcurses::Event>(mcurses::Event::Type::None);	// change this when Event becomes pure virtual
	mcurses::detail::Posted_event pe1(&obj, std::move(ev), 1);
	mcurses::detail::Posted_event pe4(&obj, std::unique_ptr<mcurses::Event>{new mcurses::Event{mcurses::Event::Type::None}}, 4);	// change this too ^^
	mcurses::detail::Posted_event pe0(&obj, std::make_unique<mcurses::Event>(mcurses::Event::Type::None), 0);

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