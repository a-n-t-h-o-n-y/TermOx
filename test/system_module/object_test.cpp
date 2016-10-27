#include <mcurses/system_module/object.hpp>
#include <mcurses/system_module/event_loop.hpp>
#include <mcurses/system_module/system.hpp>
#include <mcurses/widget_module/widget.hpp>
#include <mcurses/system_module/events/child_event.hpp>
#include <gtest/gtest.h>

#include <string>
#include <utility>

TEST(ObjectTest, DefaultConstructor)
{
	mcurses::System system; // Needed to prevent crash in other tests, static functions might be accessing non-static variables of system?
	mcurses::Object obj;
	EXPECT_TRUE(obj.children().empty());
	EXPECT_EQ(nullptr, obj.parent());
	EXPECT_TRUE(obj.is_enabled());
	EXPECT_EQ(std::string{}, obj.name());
}

TEST(ObjectTest, NameConstructor)
{
	mcurses::Object obj;
	obj.set_name("My Widget");
	EXPECT_TRUE(obj.children().empty());
	EXPECT_EQ(nullptr, obj.parent());
	EXPECT_TRUE(obj.is_enabled());
	EXPECT_EQ("My Widget", obj.name());
}

class Test_widg : public mcurses::Widget {
public:
	Test_widg(){ this->set_geometry(0,0,10,20); }
};

TEST(ObjectTest, MakeChild)
{
	mcurses::Object obj;
	obj.set_name("An Object");
	auto& child = obj.make_child<mcurses::Widget>();
	child.set_geometry(0,0,5,5);
	EXPECT_TRUE(obj.make_child<Test_widg>().has_coordinates(7, 12));

	EXPECT_EQ(2, obj.children().size());
}

TEST(ObjectTest, Children)
{
	mcurses::Object obj;
	auto& child1 = obj.make_child<mcurses::Widget>();
	child1.set_geometry(4,5,10,30);
	auto& child2 = obj.make_child<Test_widg>();
	child2.set_geometry(30,30,15,20);
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
	mcurses::Object obj;
	auto c1 = std::make_unique<mcurses::Widget>();
	c1->set_name("Widget 1");
	c1->set_geometry(0,0,4,5);
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
	mcurses::Object obj;
	obj.set_name("Parent");
	mcurses::Widget& c1 = obj.make_child<mcurses::Widget>();
	c1.set_name("Child 1");
	c1.set_geometry(0,0,50,50);
	mcurses::Object& c2 = obj.make_child<mcurses::Object>();
	c2.set_name("Child 2");

	mcurses::Widget& c1_c1 = c1.make_child<mcurses::Widget>();
	c1_c1.set_name("Child 1 - Child 1");
	c1_c1.set_geometry(0,0,7,5);
	mcurses::Widget& c1_c2 = c1.make_child<mcurses::Widget>();
	c1_c2.set_name("Child 1 - Child 2");
	c1_c2.set_geometry(10,10,3,2);
	mcurses::Object& c1_c3 = c1.make_child<mcurses::Event_loop>();
	c1_c3.set_name("Child 1 - Child 3");

	mcurses::Object& c1_c3_c1 = c1_c3.make_child<mcurses::Object>();
	c1_c3_c1.set_name("Child 1 - Child 3 - Child 1");

	mcurses::Object& c1_c3_c1_c1 = c1_c3_c1.make_child<mcurses::Event_loop>();
	c1_c3_c1_c1.set_name("Child 1");	// Duplicate name

	mcurses::Widget& c2_c1 = c2.make_child<mcurses::Widget>();
	c2_c1.set_name("Child 2 - Child 1");
	c2_c1.set_geometry(5,4,3,2);
	mcurses::Object& c2_c1_c1 = c2_c1.make_child<mcurses::Object>();
	c2_c1_c1.set_name("Child 2 - Child 1 - Child 1");

	EXPECT_EQ(&c2, obj.find_child<mcurses::Object>("Child 2"));
	EXPECT_EQ(&c1, obj.find_child<mcurses::Object>("Child 1"));
	EXPECT_EQ(&c1, obj.find_child<mcurses::Widget>("Child 1"));
	EXPECT_EQ(&c1_c3_c1_c1, obj.find_child<mcurses::Event_loop>("Child 1"));
	EXPECT_EQ(3, obj.find_child<mcurses::Widget>("Child 1")->children().size());
	EXPECT_EQ(&c1_c3_c1_c1, obj.find_child<mcurses::Event_loop>("Child 1"));
	EXPECT_EQ(nullptr, obj.find_child<mcurses::Object>("Child 12345"));
	EXPECT_EQ(&obj, obj.find_child<mcurses::Object>("Parent"));
	EXPECT_EQ(&c1_c1, obj.find_child<mcurses::Widget>("Child 1 - Child 1"));
	EXPECT_EQ(&c1_c2, obj.find_child<mcurses::Widget>("Child 1 - Child 2"));
	EXPECT_EQ(&c1_c3, obj.find_child<mcurses::Event_loop>("Child 1 - Child 3"));
	EXPECT_EQ(nullptr, obj.find_child<mcurses::Widget>("Child 1 - Child 3 - Child 1"));
	EXPECT_EQ(&c1_c3_c1, obj.find_child<mcurses::Object>("Child 1 - Child 3 - Child 1"));
	EXPECT_EQ(&c2_c1, obj.find_child<mcurses::Widget>("Child 2 - Child 1"));
	EXPECT_EQ(&c2_c1_c1, obj.find_child<mcurses::Object>("Child 2 - Child 1 - Child 1"));
	EXPECT_EQ(nullptr, obj.find_child<mcurses::Widget>("Child 2 - Child 1 - Child 1"));
}

TEST(ObjectTest, ConstFindChild)
{
	mcurses::Object obj;
	obj.set_name("Parent");
	mcurses::Widget& c1 = obj.make_child<mcurses::Widget>();
	c1.set_name("Child 1");
	c1.set_geometry(0,0,50,50);
	mcurses::Object& c2 = obj.make_child<mcurses::Object>();
	c2.set_name("Child 2");

	mcurses::Widget& c1_c1 = c1.make_child<mcurses::Widget>();
	c1_c1.set_name("Child 1 - Child 1");
	c1_c1.set_geometry(0,0,7,5);
	mcurses::Widget& c1_c2 = c1.make_child<mcurses::Widget>();
	c1_c2.set_name("Child 1 - Child 2");
	c1_c2.set_geometry(10,10,3,2);
	mcurses::Object& c1_c3 = c1.make_child<mcurses::Event_loop>();
	c1_c3.set_name("Child 1 - Child 3");

	mcurses::Object& c1_c3_c1 = c1_c3.make_child<mcurses::Object>();
	c1_c3_c1.set_name("Child 1 - Child 3 - Child 1");

	mcurses::Object& c1_c3_c1_c1 = c1_c3_c1.make_child<mcurses::Event_loop>();
	c1_c3_c1_c1.set_name("Child 1");	// Duplicate name

	mcurses::Widget& c2_c1 = c2.make_child<mcurses::Widget>();
	c2_c1.set_name("Child 2 - Child 1");
	c2_c1.set_geometry(5,4,3,2);
	mcurses::Object& c2_c1_c1 = c2_c1.make_child<mcurses::Object>();
	c2_c1_c1.set_name("Child 2 - Child 1 - Child 1");

	const mcurses::Object obj_const = std::move(obj);

	EXPECT_EQ(&c2, obj_const.find_child<mcurses::Object>("Child 2"));
	EXPECT_EQ(&c1, obj_const.find_child<mcurses::Object>("Child 1"));
	EXPECT_EQ(&c1, obj_const.find_child<mcurses::Widget>("Child 1"));
	EXPECT_EQ(&c1_c3_c1_c1, obj_const.find_child<mcurses::Event_loop>("Child 1"));
	EXPECT_EQ(3, obj_const.find_child<mcurses::Widget>("Child 1")->children().size());
	EXPECT_EQ(&c1_c3_c1_c1, obj_const.find_child<mcurses::Event_loop>("Child 1"));
	EXPECT_EQ(nullptr, obj_const.find_child<mcurses::Object>("Child 12345"));
	EXPECT_EQ(&obj_const, obj_const.find_child<mcurses::Object>("Parent"));
	EXPECT_EQ(&c1_c1, obj_const.find_child<mcurses::Widget>("Child 1 - Child 1"));
	EXPECT_EQ(&c1_c2, obj_const.find_child<mcurses::Widget>("Child 1 - Child 2"));
	EXPECT_EQ(&c1_c3, obj_const.find_child<mcurses::Event_loop>("Child 1 - Child 3"));
	EXPECT_EQ(nullptr, obj_const.find_child<mcurses::Widget>("Child 1 - Child 3 - Child 1"));
	EXPECT_EQ(&c1_c3_c1, obj_const.find_child<mcurses::Object>("Child 1 - Child 3 - Child 1"));
	EXPECT_EQ(&c2_c1, obj_const.find_child<mcurses::Widget>("Child 2 - Child 1"));
	EXPECT_EQ(&c2_c1_c1, obj_const.find_child<mcurses::Object>("Child 2 - Child 1 - Child 1"));
	EXPECT_EQ(nullptr, obj_const.find_child<mcurses::Widget>("Child 2 - Child 1 - Child 1"));
}

int glob_test_int{0};

class Event_filter_test : public mcurses::Widget {
public:
	Event_filter_test(){ this->set_geometry(0,0,5,3); }
	virtual bool event_filter(mcurses::Object* watched, mcurses::Event& event) override {
		++glob_test_int;
		return true;
	}
};

TEST(ObjectTest, InstallEventFilter)
{
	Event_filter_test test_obj;
	test_obj.install_event_filter(&test_obj);	// Does nothing

	mcurses::Widget widg;
	widg.set_geometry(0,0,4,3);
	widg.install_event_filter(&test_obj); // test_obj now is notified of events send to widg

	mcurses::Child_event ce(mcurses::Event::Type::ChildAdded, nullptr);

	mcurses::System::send_event(&widg, ce);

	EXPECT_EQ(1, glob_test_int);
}

TEST(ObjectTest, RemoveEventFilter)
{
	Event_filter_test test_obj;
	mcurses::Widget widg;
	widg.set_geometry(0,0,4,3);
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
	auto obj = std::make_shared<mcurses::Object>();
	int i{0};
	obj->destroyed.connect([&i](mcurses::Object*){++i;});
	obj.reset();
	EXPECT_EQ(1, i);
}

TEST(ObjectTest, SlotSetEnable)
{
	mcurses::Object obj;

	EXPECT_TRUE(obj.is_enabled());

	obj.enable();

	EXPECT_TRUE(obj.is_enabled());

	obj.disable();

	EXPECT_FALSE(obj.is_enabled());
}