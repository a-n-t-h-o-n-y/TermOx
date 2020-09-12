#include <system/event.hpp>
#include <system/event_loop.hpp>
#include <system/object.hpp>
#include <system/system.hpp>
#include <widget/widget.hpp>

#include <gtest/gtest.h>

#include <string>
#include <utility>

using cppurses::Child_event;
using cppurses::Event;
using cppurses::Event_loop;
using cppurses::Object;
using cppurses::System;
using cppurses::Widget;

TEST(ObjectTest, DefaultConstructor)
{
    System system;
    Object obj;
    EXPECT_TRUE(obj.children().empty());
    EXPECT_EQ(nullptr, obj.parent());
    EXPECT_TRUE(obj.enabled());
    EXPECT_EQ(std::string{}, obj.name());
}

TEST(ObjectTest, NameConstructor)
{
    System system;
    Object obj;
    obj.set_name("My Widget");
    EXPECT_TRUE(obj.children().empty());
    EXPECT_EQ(nullptr, obj.parent());
    EXPECT_TRUE(obj.enabled());
    EXPECT_EQ("My Widget", obj.name());
}

TEST(ObjectTest, MoveConstructor) {}

TEST(ObjectTest, MoveAssignmentOperator) {}

class Test_widg : public Widget {
   public:
    Test_widg()
    {
        this->set_x(0);
        this->set_y(0);
        this->geometry().set_width(10);
        this->geometry().set_height(20);
    }
};

TEST(ObjectTest, MakeChild)
{
    System system;
    Object obj;
    obj.set_name("An Object");
    auto& child = obj.make_child<Widget>();
    child.set_x(0);
    child.set_y(0);
    child.geometry().set_width(5);
    child.geometry().set_height(5);
    obj.make_child<Test_widg>();

    EXPECT_EQ(2, obj.children().size());
}

TEST(ObjectTest, Children)
{
    System system;
    Object obj;
    auto& child1 = obj.make_child<Widget>();
    child1.set_x(4);
    child1.set_y(5);
    child1.geometry().set_width(10);
    child1.geometry().set_height(30);
    auto& child2 = obj.make_child<Test_widg>();
    child2.set_x(30);
    child2.set_y(30);
    child2.geometry().set_width(15);
    child2.geometry().set_height(20);
    obj.make_child<Event_loop>();
    auto children = obj.children();
    ASSERT_EQ(3, children.size());
    EXPECT_TRUE(children[0]->enabled());
    EXPECT_TRUE(children[1]->enabled());
    EXPECT_TRUE(children[2]->enabled());
}

TEST(ObjectTest, AddChild)
{
    System system;
    Object obj;
    auto c1 = std::make_unique<Widget>();
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
    System system;
    Object obj;
    obj.set_name("Parent");
    Widget& c1 = obj.make_child<Widget>();
    c1.set_name("Child 1");
    c1.set_x(0);
    c1.set_y(0);
    c1.geometry().set_width(50);
    c1.geometry().set_height(50);
    Object& c2 = obj.make_child<Object>();
    c2.set_name("Child 2");

    Widget& c1_c1 = c1.make_child<Widget>();
    c1_c1.set_name("Child 1 - Child 1");
    c1_c1.set_x(0);
    c1_c1.set_y(0);
    c1_c1.geometry().set_width(7);
    c1_c1.geometry().set_height(5);
    Widget& c1_c2 = c1.make_child<Widget>();
    c1_c2.set_name("Child 1 - Child 2");
    c1_c2.set_x(10);
    c1_c2.set_y(10);
    c1_c2.geometry().set_width(3);
    c1_c2.geometry().set_height(2);
    Object& c1_c3 = c1.make_child<Event_loop>();
    c1_c3.set_name("Child 1 - Child 3");

    Object& c1_c3_c1 = c1_c3.make_child<Object>();
    c1_c3_c1.set_name("Child 1 - Child 3 - Child 1");

    Object& c1_c3_c1_c1 = c1_c3_c1.make_child<Event_loop>();
    c1_c3_c1_c1.set_name("Child 1");  // Duplicate name

    Widget& c2_c1 = c2.make_child<Widget>();
    c2_c1.set_name("Child 2 - Child 1");
    c2_c1.set_x(5);
    c2_c1.set_y(4);
    c2_c1.geometry().set_width(3);
    c2_c1.geometry().set_height(2);
    Object& c2_c1_c1 = c2_c1.make_child<Object>();
    c2_c1_c1.set_name("Child 2 - Child 1 - Child 1");

    EXPECT_EQ(&c2, obj.find_child<Object>("Child 2"));
    EXPECT_EQ(&c1, obj.find_child<Object>("Child 1"));
    EXPECT_EQ(&c1, obj.find_child<Widget>("Child 1"));
    EXPECT_EQ(&c1_c3_c1_c1, obj.find_child<Event_loop>("Child 1"));
    EXPECT_EQ(3, obj.find_child<Widget>("Child 1")->children().size());
    EXPECT_EQ(&c1_c3_c1_c1, obj.find_child<Event_loop>("Child 1"));
    EXPECT_EQ(nullptr, obj.find_child<Object>("Child 12345"));
    EXPECT_EQ(&obj, obj.find_child<Object>("Parent"));
    EXPECT_EQ(&c1_c1, obj.find_child<Widget>("Child 1 - Child 1"));
    EXPECT_EQ(&c1_c2, obj.find_child<Widget>("Child 1 - Child 2"));
    EXPECT_EQ(&c1_c3, obj.find_child<Event_loop>("Child 1 - Child 3"));
    EXPECT_EQ(nullptr, obj.find_child<Widget>("Child 1 - Child 3 - Child 1"));
    EXPECT_EQ(&c1_c3_c1, obj.find_child<Object>("Child 1 - Child 3 - Child 1"));
    EXPECT_EQ(&c2_c1, obj.find_child<Widget>("Child 2 - Child 1"));
    EXPECT_EQ(&c2_c1_c1, obj.find_child<Object>("Child 2 - Child 1 - Child 1"));
    EXPECT_EQ(nullptr, obj.find_child<Widget>("Child 2 - Child 1 - Child 1"));
}

TEST(ObjectTest, ConstFindChild)
{
    System system;
    Object obj;
    obj.set_name("Parent");
    Widget& c1 = obj.make_child<Widget>();
    c1.set_name("Child 1");
    c1.set_x(0);
    c1.set_y(0);
    c1.geometry().set_width(50);
    c1.geometry().set_height(50);
    Object& c2 = obj.make_child<Object>();
    c2.set_name("Child 2");

    Widget& c1_c1 = c1.make_child<Widget>();
    c1_c1.set_name("Child 1 - Child 1");
    c1_c1.set_x(0);
    c1_c1.set_y(0);
    c1_c1.geometry().set_width(7);
    c1_c1.geometry().set_height(5);
    Widget& c1_c2 = c1.make_child<Widget>();
    c1_c2.set_name("Child 1 - Child 2");
    c1_c2.set_x(10);
    c1_c2.set_y(10);
    c1_c2.geometry().set_width(3);
    c1_c2.geometry().set_height(2);
    Object& c1_c3 = c1.make_child<Event_loop>();
    c1_c3.set_name("Child 1 - Child 3");

    Object& c1_c3_c1 = c1_c3.make_child<Object>();
    c1_c3_c1.set_name("Child 1 - Child 3 - Child 1");

    Object& c1_c3_c1_c1 = c1_c3_c1.make_child<Event_loop>();
    c1_c3_c1_c1.set_name("Child 1");  // Duplicate name

    Widget& c2_c1 = c2.make_child<Widget>();
    c2_c1.set_name("Child 2 - Child 1");
    c2_c1.set_x(5);
    c2_c1.set_y(4);
    c2_c1.geometry().set_width(3);
    c2_c1.geometry().set_height(2);
    Object& c2_c1_c1 = c2_c1.make_child<Object>();
    c2_c1_c1.set_name("Child 2 - Child 1 - Child 1");

    const Object obj_const = std::move(obj);
    EXPECT_EQ(&c2, obj_const.find_child<const Object>("Child 2"));
    EXPECT_EQ(&c1, obj_const.find_child<const Object>("Child 1"));
    EXPECT_EQ(&c1, obj_const.find_child<const Widget>("Child 1"));
    EXPECT_EQ(&c1_c3_c1_c1, obj_const.find_child<const Event_loop>("Child 1"));
    EXPECT_EQ(3,
              obj_const.find_child<const Widget>("Child 1")->children().size());
    EXPECT_EQ(&c1_c3_c1_c1, obj_const.find_child<const Event_loop>("Child 1"));
    EXPECT_EQ(nullptr, obj_const.find_child<const Object>("Child 12345"));
    EXPECT_EQ(&obj_const, obj_const.find_child<const Object>("Parent"));
    EXPECT_EQ(&c1_c1, obj_const.find_child<const Widget>("Child 1 - Child 1"));
    EXPECT_EQ(&c1_c2, obj_const.find_child<const Widget>("Child 1 - Child 2"));
    EXPECT_EQ(&c1_c3,
              obj_const.find_child<const Event_loop>("Child 1 - Child 3"));
    EXPECT_EQ(nullptr, obj_const.find_child<const Widget>(
                           "Child 1 - Child 3 - Child 1"));
    EXPECT_EQ(&c1_c3_c1, obj_const.find_child<const Object>(
                             "Child 1 - Child 3 - Child 1"));
    EXPECT_EQ(&c2_c1, obj_const.find_child<const Widget>("Child 2 - Child 1"));
    EXPECT_EQ(&c2_c1_c1, obj_const.find_child<const Object>(
                             "Child 2 - Child 1 - Child 1"));
    EXPECT_EQ(nullptr, obj_const.find_child<const Widget>(
                           "Child 2 - Child 1 - Child 1"));
}

int glob_test_int{0};

class Event_filter_test : public Widget {
   public:
    Event_filter_test()
    {
        this->set_x(0);
        this->set_y(0);
        this->geometry().set_width(5);
        this->geometry().set_height(3);
    }
    bool event_filter(Object* watched, const Event& event) override
    {
        ++glob_test_int;
        return true;
    }
};

TEST(ObjectTest, InstallEventFilter)
{
    System system;
    Event_filter_test test_obj;
    test_obj.install_event_filter(&test_obj);  // Does nothing

    Widget widg;
    widg.set_x(0);
    widg.set_y(0);
    widg.geometry().set_width(4);
    widg.geometry().set_height(3);
    widg.install_event_filter(
        &test_obj);  // test_obj now is notified of events send to widg

    Child_event ce(Event::ChildAdded, nullptr);

    System::send_event(&widg, ce);

    EXPECT_EQ(1, glob_test_int);
}

TEST(ObjectTest, RemoveEventFilter)
{
    System system;
    Event_filter_test test_obj;
    Widget widg;
    widg.set_x(0);
    widg.set_y(0);
    widg.geometry().set_width(4);
    widg.geometry().set_height(3);
    widg.install_event_filter(&test_obj);

    Child_event ce(Event::ChildAdded, nullptr);

    System::send_event(&widg, ce);
    EXPECT_EQ(2, glob_test_int);

    widg.remove_event_filter(&test_obj);
    System::send_event(&widg, ce);
    EXPECT_EQ(2, glob_test_int);
}

TEST(ObjectTest, SignalObjectNameChanged)
{
    System system;
    Object obj;
    obj.set_name("NameOne");
    int i{0};
    obj.object_name_changed.connect([&i](const std::string& s) { ++i; });
    obj.set_name("NameTwo");
    EXPECT_EQ("NameTwo", obj.name());
    EXPECT_EQ(1, i);
}

TEST(ObjectTest, SignalDestroyed)
{
    System system;
    auto obj = std::make_shared<Object>();
    int i{0};
    obj->destroyed.connect([&i](Object*) { ++i; });
    obj.reset();
    EXPECT_EQ(1, i);
}

TEST(ObjectTest, SlotSetEnable)
{
    System system;
    Object obj;

    EXPECT_TRUE(obj.enabled());

    obj.enable();

    EXPECT_TRUE(obj.enabled());

    obj.disable();

    EXPECT_FALSE(obj.enabled());
}
