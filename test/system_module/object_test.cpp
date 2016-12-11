#include <system_module/object.hpp>
#include <system_module/event_loop.hpp>
#include <system_module/system.hpp>
#include <system_module/events/child_event.hpp>
#include <widget_module/widget.hpp>

#include <gtest/gtest.h>

#include <string>
#include <utility>

TEST(ObjectTest, DefaultConstructor)
{
	mcurses::System system;
	mcurses::Object obj;
	EXPECT_TRUE(obj.children().empty());
	EXPECT_EQ(nullptr, obj.parent());
	EXPECT_TRUE(obj.is_enabled());
	EXPECT_EQ(std::string{}, obj.name());
}

TEST(ObjectTest, NameConstructor)
{
	mcurses::System system;
	mcurses::Object obj;
	obj.set_name("My Widget");
	EXPECT_TRUE(obj.children().empty());
	EXPECT_EQ(nullptr, obj.parent());
	EXPECT_TRUE(obj.is_enabled());
	EXPECT_EQ("My Widget", obj.name());
}

class Test_widg : public mcurses::Widget {
public:
	Test_widg(){
		this->set_x(0);
		this->set_y(0);
		this->geometry().set_width(10);
		this->geometry().set_height(20);
	}
};

TEST(ObjectTest, MakeChild)
{
	mcurses::System system;
	mcurses::Object obj;
	obj.set_name("An Object");
	auto& child = obj.make_child<mcurses::Widget>();
	child.set_x(0);
	child.set_y(0);
	child.geometry().set_width(5);
	child.geometry().set_height(5);
	EXPECT_TRUE(obj.make_child<Test_widg>().has_coordinates(7, 12));

	EXPECT_EQ(2, obj.children().size());
}

TEST(ObjectTest, Children)
{
	mcurses::System system;
	mcurses::Object obj;
	auto& child1 = obj.make_child<mcurses::Widget>();
	child1.set_x(4);
	child1.set_y(5);
	child1.geometry().set_width(10);
	child1.geometry().set_height(30);
	auto& child2 = obj.make_child<Test_widg>();
	child2.set_x(30);
	child2.set_y(30);
	child2.geometry().set_width(15);
	child2.geometry().set_height(20);
	obj.make_child<mcurses::Event_loop>();
	auto children = obj.children();
	ASSERT_EQ(3, children.size());
	EXPECT_TRUE(children[0]->is_enabled());
	EXPECT_FALSE(children[1]->has_coordinates(8,13));
	EXPECT_TRUE(children[1]->has_coordinates(40,42));
	EXPECT_TRUE(children[2]->is_enabled());
}

TEST(ObjectTest, AddChild)
{
	mcurses::System system;
	mcurses::Object obj;
	auto c1 = std::make_unique<mcurses::Widget>();
	c1->set_name("Widget 1");
	c1->set_x(0);
	c1->set_y(0);
	c1->geometry().set_width(4);
	c1->geometry().set_height(5);
	auto c2 = std::make_unique<Test_widg>();
	c2->set_name("Widget 2");

	obj.add_child(std::move(c1));
	obj.add_child(std::move(c2));

	auto children = obj.children();

	ASSERT_EQ(2, children.size());

	EXPECT_EQ("Widget 1", children[0]->name());
	EXPECT_EQ("Widget 2", children[1]->name());
}

TEST(ObjectTest, FindChild)
{
	mcurses::System system;
	mcurses::Object obj;
	obj.set_name("Parent");
	mcurses::Widget& c1 = obj.make_child<mcurses::Widget>();
	c1.set_name("Child 1");
	c1.set_x(0);
	c1.set_y(0);
	c1.geometry().set_width(50);
	c1.geometry().set_height(50);
	mcurses::Object& c2 = obj.make_child<mcurses::Object>();
	c2.set_name("Child 2");

	mcurses::Widget& c1_c1 = c1.make_child<mcurses::Widget>();
	c1_c1.set_name("Child 1 - Child 1");
	c1_c1.set_x(0);
	c1_c1.set_y(0);
	c1_c1.geometry().set_width(7);
	c1_c1.geometry().set_height(5);
	mcurses::Widget& c1_c2 = c1.make_child<mcurses::Widget>();
	c1_c2.set_name("Child 1 - Child 2");
	c1_c2.set_x(10);
	c1_c2.set_y(10);
	c1_c2.geometry().set_width(3);
	c1_c2.geometry().set_height(2);
	mcurses::Object& c1_c3 = c1.make_child<mcurses::Event_loop>();
	c1_c3.set_name("Child 1 - Child 3");

	mcurses::Object& c1_c3_c1 = c1_c3.make_child<mcurses::Object>();
    // Seg fault here
	// c1_c3_c1.set_name("Child 1 - Child 3 - Child 1");

	// mcurses::Object& c1_c3_c1_c1 = c1_c3_c1.make_child<mcurses::Event_loop>();
	// c1_c3_c1_c1.set_name("Child 1");	// Duplicate name

	// mcurses::Widget& c2_c1 = c2.make_child<mcurses::Widget>();
	// c2_c1.set_name("Child 2 - Child 1");
	// c2_c1.set_x(5);
	// c2_c1.set_y(4);
	// c2_c1.geometry().set_width(3);
	// c2_c1.geometry().set_height(2);
	// mcurses::Object& c2_c1_c1 = c2_c1.make_child<mcurses::Object>();
	// c2_c1_c1.set_name("Child 2 - Child 1 - Child 1");

	// EXPECT_EQ(&c2, obj.find_child<mcurses::Object>("Child 2"));
	// EXPECT_EQ(&c1, obj.find_child<mcurses::Object>("Child 1"));
	// EXPECT_EQ(&c1, obj.find_child<mcurses::Widget>("Child 1"));
	// EXPECT_EQ(&c1_c3_c1_c1, obj.find_child<mcurses::Event_loop>("Child 1"));
	// EXPECT_EQ(3, obj.find_child<mcurses::Widget>("Child 1")->children().size());
	// EXPECT_EQ(&c1_c3_c1_c1, obj.find_child<mcurses::Event_loop>("Child 1"));
	// EXPECT_EQ(nullptr, obj.find_child<mcurses::Object>("Child 12345"));
	// EXPECT_EQ(&obj, obj.find_child<mcurses::Object>("Parent"));
	// EXPECT_EQ(&c1_c1, obj.find_child<mcurses::Widget>("Child 1 - Child 1"));
	// EXPECT_EQ(&c1_c2, obj.find_child<mcurses::Widget>("Child 1 - Child 2"));
	// EXPECT_EQ(&c1_c3, obj.find_child<mcurses::Event_loop>("Child 1 - Child 3"));
	// EXPECT_EQ(nullptr, obj.find_child<mcurses::Widget>("Child 1 - Child 3 - Child 1"));
	// EXPECT_EQ(&c1_c3_c1, obj.find_child<mcurses::Object>("Child 1 - Child 3 - Child 1"));
	// EXPECT_EQ(&c2_c1, obj.find_child<mcurses::Widget>("Child 2 - Child 1"));
	// EXPECT_EQ(&c2_c1_c1, obj.find_child<mcurses::Object>("Child 2 - Child 1 - Child 1"));
	// EXPECT_EQ(nullptr, obj.find_child<mcurses::Widget>("Child 2 - Child 1 - Child 1"));
}

TEST(ObjectTest, ConstFindChild)
{
	mcurses::System system;
	mcurses::Object obj;
	obj.set_name("Parent");
	mcurses::Widget& c1 = obj.make_child<mcurses::Widget>();
	c1.set_name("Child 1");
	c1.set_x(0);
	c1.set_y(0);
	c1.geometry().set_width(50);
	c1.geometry().set_height(50);
	mcurses::Object& c2 = obj.make_child<mcurses::Object>();
	c2.set_name("Child 2");

	mcurses::Widget& c1_c1 = c1.make_child<mcurses::Widget>();
	c1_c1.set_name("Child 1 - Child 1");
	c1_c1.set_x(0);
	c1_c1.set_y(0);
	c1_c1.geometry().set_width(7);
	c1_c1.geometry().set_height(5);
	mcurses::Widget& c1_c2 = c1.make_child<mcurses::Widget>();
	c1_c2.set_name("Child 1 - Child 2");
	c1_c2.set_x(10);
	c1_c2.set_y(10);
	c1_c2.geometry().set_width(3);
	c1_c2.geometry().set_height(2);
	mcurses::Object& c1_c3 = c1.make_child<mcurses::Event_loop>();
	c1_c3.set_name("Child 1 - Child 3");

	mcurses::Object& c1_c3_c1 = c1_c3.make_child<mcurses::Object>();
    // Seg fault here
/*     c1_c3_c1.set_name("Child 1 - Child 3 - Child 1");
 * 
 *     mcurses::Object& c1_c3_c1_c1 = c1_c3_c1.make_child<mcurses::Event_loop>();
 *     c1_c3_c1_c1.set_name("Child 1");	// Duplicate name
 * 
 *     mcurses::Widget& c2_c1 = c2.make_child<mcurses::Widget>();
 *     c2_c1.set_name("Child 2 - Child 1");
 *     c2_c1.set_x(5);
 *     c2_c1.set_y(4);
 *     c2_c1.geometry().set_width(3);
 *     c2_c1.geometry().set_height(2);
 *     mcurses::Object& c2_c1_c1 = c2_c1.make_child<mcurses::Object>();
 *     c2_c1_c1.set_name("Child 2 - Child 1 - Child 1");
 * 
 *     const mcurses::Object obj_const = std::move(obj);
 * 
 *     EXPECT_EQ(&c2, obj_const.find_child<mcurses::Object>("Child 2"));
 *     EXPECT_EQ(&c1, obj_const.find_child<mcurses::Object>("Child 1"));
 *     EXPECT_EQ(&c1, obj_const.find_child<mcurses::Widget>("Child 1"));
 *     EXPECT_EQ(&c1_c3_c1_c1, obj_const.find_child<mcurses::Event_loop>("Child 1"));
 *     EXPECT_EQ(3, obj_const.find_child<mcurses::Widget>("Child 1")->children().size());
 *     EXPECT_EQ(&c1_c3_c1_c1, obj_const.find_child<mcurses::Event_loop>("Child 1"));
 *     EXPECT_EQ(nullptr, obj_const.find_child<mcurses::Object>("Child 12345"));
 *     EXPECT_EQ(&obj_const, obj_const.find_child<mcurses::Object>("Parent"));
 *     EXPECT_EQ(&c1_c1, obj_const.find_child<mcurses::Widget>("Child 1 - Child 1"));
 *     EXPECT_EQ(&c1_c2, obj_const.find_child<mcurses::Widget>("Child 1 - Child 2"));
 *     EXPECT_EQ(&c1_c3, obj_const.find_child<mcurses::Event_loop>("Child 1 - Child 3"));
 *     EXPECT_EQ(nullptr, obj_const.find_child<mcurses::Widget>("Child 1 - Child 3 - Child 1"));
 *     EXPECT_EQ(&c1_c3_c1, obj_const.find_child<mcurses::Object>("Child 1 - Child 3 - Child 1"));
 *     EXPECT_EQ(&c2_c1, obj_const.find_child<mcurses::Widget>("Child 2 - Child 1"));
 *     EXPECT_EQ(&c2_c1_c1, obj_const.find_child<mcurses::Object>("Child 2 - Child 1 - Child 1"));
 *     EXPECT_EQ(nullptr, obj_const.find_child<mcurses::Widget>("Child 2 - Child 1 - Child 1")); */
}

int glob_test_int{0};

class Event_filter_test : public mcurses::Widget {
public:
	Event_filter_test(){
		this->set_x(0);
		this->set_y(0);
		this->geometry().set_width(5);
		this->geometry().set_height(3);
	}
	virtual bool event_filter(mcurses::Object* watched, mcurses::Event& event) override {
		++glob_test_int;
		return true;
	}
};

TEST(ObjectTest, InstallEventFilter)
{
	mcurses::System system;
	Event_filter_test test_obj;
	test_obj.install_event_filter(&test_obj);	// Does nothing

	mcurses::Widget widg;
	widg.set_x(0);
	widg.set_y(0);
	widg.geometry().set_width(4);
	widg.geometry().set_height(3);
	widg.install_event_filter(&test_obj); // test_obj now is notified of events send to widg

	mcurses::Child_event ce(mcurses::Event::Type::ChildAdded, nullptr);

	mcurses::System::send_event(&widg, ce);

	EXPECT_EQ(1, glob_test_int);
}

TEST(ObjectTest, RemoveEventFilter)
{
	mcurses::System system;
	Event_filter_test test_obj;
	mcurses::Widget widg;
	widg.set_x(0);
	widg.set_y(0);
	widg.geometry().set_width(4);
	widg.geometry().set_height(3);
	widg.install_event_filter(&test_obj);

	mcurses::Child_event ce(mcurses::Event::Type::ChildAdded, nullptr);

	mcurses::System::send_event(&widg, ce);
	EXPECT_EQ(2, glob_test_int);

	widg.remove_event_filter(&test_obj);
	mcurses::System::send_event(&widg, ce);
	EXPECT_EQ(2, glob_test_int);
}

TEST(ObjectTest, SignalObjectNameChanged)
{
	mcurses::System system;
	mcurses::Object obj;\
	obj.set_name("NameOne");
	int i{0};
	obj.object_name_changed.connect([&i](const std::string& s){++i;});
	obj.set_name("NameTwo");
	EXPECT_EQ("NameTwo", obj.name());
	EXPECT_EQ(1, i);
}

TEST(ObjectTest, SignalDestroyed)
{
	mcurses::System system;
	auto obj = std::make_shared<mcurses::Object>();
	int i{0};
	obj->destroyed.connect([&i](mcurses::Object*){++i;});
	obj.reset();
	EXPECT_EQ(1, i);
}

TEST(ObjectTest, SlotSetEnable)
{
	mcurses::System system;
	mcurses::Object obj;

	EXPECT_TRUE(obj.is_enabled());

	obj.enable();

	EXPECT_TRUE(obj.is_enabled());

	obj.disable();

	EXPECT_FALSE(obj.is_enabled());
}
