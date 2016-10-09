#include <mcurses/signal_module/slot.hpp>
#include <mcurses/signal_module/expired_slot.hpp>
#include <mcurses/signal_module/signal.hpp>
#include <mcurses/signal_module/optional.hpp>
#include <gtest/gtest.h>
#include <boost/function.hpp>
#include <typeinfo>
#include <memory>
#include <functional>

TEST(SlotTest, Arity)
{
	mcurses::slot<void(int, int, double, float)> a_slot = {[](int,int,double,float){return;}};

	EXPECT_EQ(4, a_slot.arity);

	EXPECT_EQ(3, mcurses::slot<int(char, int, double)>::arity);
}

TEST(SlotTest, ArgumentNTypeAccess)
{
	typedef mcurses::slot<long(int, double, float, float, char, bool)>::arg<0>::type type_zero;
	typedef mcurses::slot<long(int, double, float, float, char, bool)>::arg<1>::type type_one;
	typedef mcurses::slot<long(int, double, float, float, char, bool)>::arg<2>::type type_two;
	typedef mcurses::slot<long(int, double, float, float, char, bool)>::arg<3>::type type_three;
	typedef mcurses::slot<long(int, double, float, float, char, bool)>::arg<4>::type type_four;
	typedef mcurses::slot<long(int, double, float, float, char, bool)>::arg<5>::type type_five;
	EXPECT_EQ(typeid(int), typeid(type_zero));
	EXPECT_EQ(typeid(double), typeid(type_one));
	EXPECT_EQ(typeid(float), typeid(type_two));
	EXPECT_EQ(typeid(float), typeid(type_three));
	EXPECT_EQ(typeid(char), typeid(type_four));
	EXPECT_EQ(typeid(bool), typeid(type_five));
}

TEST(SlotTest, DefaultConstructor)
{
	mcurses::slot<void(int)> s;
	EXPECT_TRUE(s.slot_function() == nullptr);
}

TEST(SlotTest, CopyAssignmentOperator)
{
	mcurses::slot<int(int, double)> s;
	mcurses::slot<int(int, double)> s2{[](int, double){return 3;}};

	auto i = std::make_shared<int>(6);

	s2.track(i);

	s = s2;

	EXPECT_EQ(3, s(6,4.3));

	EXPECT_FALSE(s.expired());

	i.reset();

	EXPECT_TRUE(s.expired());
}

TEST(SlotTest, WithDefaultFunctionType)
{
	mcurses::slot<double(int, double)> a_slot = {[](int i, double d){return i+d;}};

	EXPECT_DOUBLE_EQ(7.2, a_slot(4,3.2));
}

TEST(SlotTest, WithBoostFunctionType)
{
	mcurses::slot<int(int), boost::function<int(int)>> a_slot{[](int i){return i*2;}};

	EXPECT_EQ(4, a_slot(2));
}

// Update with checks once implemented(?)
TEST(SlotTest, TrackWeakPointer)
{
	mcurses::slot<void(int, double)> s = {[](int, double){return;}};
	auto s_i = std::make_shared<int>(5);
	s.track(s_i);
	s.track(std::make_shared<char>('g'));
}

TEST(SlotTest, TrackSignal)
{
	auto sig = std::make_shared<mcurses::signal<void(int)>>();
	sig->connect([](int){return;});

	mcurses::slot<char(int,double)> slt = [](int,double){return 'k';};

	slt.track(*sig);

	EXPECT_FALSE(slt.expired());

	sig.reset();

	EXPECT_TRUE(slt.expired());

}

TEST(SlotTest, TrackSlot)
{
	mcurses::slot<void(int)> s1 = [](int){return;};
	mcurses::slot<void(int)> s2 = [](int){return;};
	mcurses::slot<double(char, int)> s3 = [](char,int){return 3.4;};
	mcurses::slot<void(int)> s4 = [](int){return;};

	auto obj1 = std::make_shared<int>(5);
	s1.track(obj1);

	auto obj2 = std::make_shared<char>('k');
	s2.track(obj2);

	EXPECT_FALSE(s1.expired());
	EXPECT_FALSE(s2.expired());
	EXPECT_FALSE(s3.expired());

	s3.track(s1);
	EXPECT_FALSE(s1.expired());
	EXPECT_FALSE(s3.expired());

	obj1.reset();
	EXPECT_TRUE(s1.expired());
	EXPECT_TRUE(s3.expired());

	EXPECT_FALSE(s2.expired());

	obj2.reset();
	s4.track(s2);
	EXPECT_TRUE(s4.expired());
}

TEST(SlotTest, SlotFunction)
{
	mcurses::slot<int(char, double)> s{[](char, double){return 5;}};

	EXPECT_EQ(5, s.slot_function()('g',3.7));

	s.slot_function() = [](char, double){return 7;};

	EXPECT_EQ(7, s('f', 2.8));

	// void return type
	mcurses::slot<void(char, double)> s_v{[](char, double){return 5;}};
	s_v.slot_function()('h', 7.3);
}

TEST(SlotTest, ConstSlotFunction)
{
	const mcurses::slot<int(char, double)> s{[](char, double){return 5;}};

	EXPECT_EQ(5, s.slot_function()('g',3.7));
}

// Update
TEST(SlotTest, ConstructWithSignal)
{
	mcurses::signal<int(int)> sig;
	mcurses::slot<mcurses::optional<int>(int)> a_slot{sig};
}

TEST(SlotTest, CopyConstructor)
{
	mcurses::slot<double(int)> s{[](int){return 7.3;}};
	auto obj = std::make_shared<char>('u');
	s.track(obj);
	mcurses::slot<double(int)> s_copy{s};

	EXPECT_DOUBLE_EQ(7.3, s_copy(9));

	EXPECT_FALSE(s.expired());
	EXPECT_FALSE(s_copy.expired());

	obj.reset();

	EXPECT_TRUE(s.expired());
	EXPECT_TRUE(s.expired());
}

TEST(SlotTest, BindWithConstructor)
{
	auto lmda = [](int i, double d, char c){return i + d;};

	mcurses::slot<double(char)> s{std::bind(lmda, 5, 2.3, std::placeholders::_1)};

	EXPECT_EQ(7.3, s('k'));
	// s(2.3, 'h');
}

TEST(SlotTest, ParenthesisOperator)
{
	// result returned
	mcurses::slot<int(int, int)> s{[](int i, int ii){return i+ii;}};

	auto obj = std::make_shared<double>(8.3);
	s.track(obj);

	EXPECT_EQ(7, s(4,3));

	// objects locked - obj is not deleted in separate thread(?)

	// exception thrown
	obj.reset();
	EXPECT_THROW(s(4,5), mcurses::expired_slot);

	// void return type
	mcurses::slot<void(char, double)> s_v{[](char, double){return 5;}};
	s_v('h', 7.3);
}

// How is this different than above?
TEST(SlotTest, ConstParenthesisOperator)
{
	// result returned
	const mcurses::slot<int(int, int)> s{[](int i, int ii){return i+ii;}};

	// auto obj = std::make_shared<double>(8.3);
	// s.track(obj);
	// can't track object in a const object, because it alters the slot(non-const op)

	EXPECT_EQ(7, s(4,3));

	// objects locked - obj is not deleted in separate thread(?)

	// exception thrown
	// obj.reset();
	// EXPECT_THROW(s(4,5), mcurses::expired_slot);
}