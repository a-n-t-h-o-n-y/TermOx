#include <mcurses/system_module/object.hpp>
#include <mcurses/system_module/event_loop.hpp>
#include <mcurses/widget_module/widget.hpp>
#include <gtest/gtest.h>

#include <string>

TEST(ObjectTest, DefaultConstructor)
{
	mcurses::Object obj;
	EXPECT_TRUE(obj.children().empty());
	EXPECT_EQ(nullptr, obj.parent());
	EXPECT_TRUE(obj.is_enabled());
	EXPECT_EQ(std::string{}, obj.name());
}

TEST(ObjectTest, NameConstructor)
{
	mcurses::Object obj{"My Widget"};
	EXPECT_TRUE(obj.children().empty());
	EXPECT_EQ(nullptr, obj.parent());
	EXPECT_TRUE(obj.is_enabled());
	EXPECT_EQ("My Widget", obj.name());
}

class Test_widg : public mcurses::Widget {
public:
	Test_widg():Widget(0,0,10,20){}
};

TEST(ObjectTest, MakeChild)
{
	mcurses::Object obj{"An Object"};
	obj.make_child<mcurses::Widget>(0,0,5,5);
	EXPECT_TRUE(obj.make_child<Test_widg>().has_coordinates(7, 12));

	EXPECT_EQ(2, obj.children().size());
}

TEST(ObjectTest, Children)
{
	mcurses::Object obj;
	obj.make_child<mcurses::Widget>(4,5,10,30);
	obj.make_child<Test_widg>();
	obj.make_child<mcurses::Event_loop>();
	auto children = obj.children();
	ASSERT_EQ(3, children.size());
	EXPECT_TRUE(children[0]->is_enabled());
	EXPECT_TRUE(children[1]->has_coordinates(8,13));
	EXPECT_FALSE(children[1]->has_coordinates(40,42));
	EXPECT_TRUE(children[2]->is_enabled());
}

TEST(ObjectTest, AddChild)
{
	mcurses::Object obj;
	auto c1 = std::make_unique<mcurses::Widget>(0,0,4,5);
	c1->set_name("Widget 1");
	auto c2 = std::make_unique<Test_widg>();
	c2->set_name("Widget 2");

	obj.add_child(std::move(c1));
	obj.add_child(std::move(c2));

	auto children = obj.children();

	ASSERT_EQ(2, children.size());

	EXPECT_EQ("Widget 1", children[0]->name());
	EXPECT_EQ("Widget 2", children[1]->name());
}

