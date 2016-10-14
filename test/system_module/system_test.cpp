#include <mcurses/system_module/system.hpp>
#include <mcurses/system_module/event.hpp>
#include <mcurses/system_module/object.hpp>
#include <gtest/gtest.h>

#include <memory>

TEST(SystemTest, DefaultConstructor)
{
	mcurses::System system;
	EXPECT_EQ(0, system.run());
}

TEST(SystemTest, PostEvent)
{
	mcurses::Object obj;
	mcurses::System::post_event(&obj, std::make_unique<mcurses::Event>());
}