#include <mcurses/signal_module/signal.hpp>
#include <mcurses/signal_module/optional.hpp>
#include <mcurses/signal_module/optional_last_value.hpp>
#include <mcurses/signal_module/slot.hpp>
#include <mcurses/signal_module/position.hpp>

#include <gtest/gtest.h>
#include <functional>
#include <typeinfo>
#include <memory>

TEST(SignalTest, Arity)
{
	mcurses::signal<int(double, float, char, bool)> sig;
	EXPECT_EQ(4, sig.arity);

	mcurses::signal<int()> sig2;
	EXPECT_EQ(0, sig2.arity);

	mcurses::signal<void()> sig3;
	EXPECT_EQ(0, sig3.arity);

	EXPECT_EQ(2, mcurses::signal<void(int, double)>::arity);
}

TEST(SignalTest, Constructor)
{
	mcurses::signal<double(int, char)> sig1;
	EXPECT_TRUE(sig1.empty());
	EXPECT_FALSE(bool(sig1(5, 'h')));	// returns an empty optional<double>

	mcurses::signal<void(int)> sig2;
	EXPECT_TRUE(sig2.empty());
	// EXPECT_FALSE(bool(sig2(5))); // returns void

	mcurses::signal<void()> sig3;
	EXPECT_TRUE(sig3.empty());
	// EXPECT_FALSE(bool(sig3())); // returns void

	mcurses::signal<void(char, double), mcurses::optional_last_value<void>> sig4;
	EXPECT_TRUE(sig4.empty());
	// EXPECT_FALSE(bool(sig4('h', 8.4))); // returns void
	auto comb_void = mcurses::optional_last_value<void>();
	EXPECT_EQ(typeid(comb_void), typeid(sig4.combiner()));

	const mcurses::signal<char(int, double, float), mcurses::optional_last_value<char>, std::less<char>> sig5;
	EXPECT_TRUE(sig5.empty());
	EXPECT_FALSE(bool(sig5(4, 8.4, 6.5))); // returns optional<char>
	auto comb_char = mcurses::optional_last_value<char>();
	EXPECT_EQ(typeid(comb_char), typeid(sig5.combiner()));
}

TEST(SignalTest, SignalInSlotConnectedToSignal)
{
	mcurses::signal<int(double, char)> sig_child;
	mcurses::slot<int(double, char)> slot_1 = [](double, char){return 1;};
	sig_child.connect(slot_1);
	mcurses::slot<int(double, char)> slot_2 = [](double, char){return 2;};
	sig_child.connect(slot_2, mcurses::position::at_front);
	mcurses::slot<int(double, char)> slot_3 = [](double, char){return 3;};
	sig_child.connect(slot_3);

	mcurses::slot<mcurses::optional<int>(double, char)> slot_holding_signal = sig_child;

	mcurses::signal<mcurses::optional<int>(double, char)> sig_parent;
	sig_parent.connect(slot_holding_signal);

	// This slot will not be called by the signal containing sig_child
	mcurses::slot<int(double, char)> slot_4 = [](double, char){return 4;};
	sig_child.connect(slot_4);

	auto result1 = sig_parent(7.3, 'h');

	ASSERT_TRUE(bool(result1));
	ASSERT_TRUE(bool(*result1));
	EXPECT_EQ(3, **result1);

	auto result2 = sig_parent(7.3, 'h');

	ASSERT_TRUE(bool(result2));
	ASSERT_TRUE(bool(*result2));
	EXPECT_EQ(3, **result2);
	EXPECT_EQ(1, sig_parent.num_slots());

	auto result3 = sig_child(3.2, 'k');

	ASSERT_TRUE(bool(result3));
	EXPECT_EQ(4, *result3);
	EXPECT_EQ(4, sig_child.num_slots());
}

TEST(SignalTest, MoveConstructor)
{
	mcurses::signal<char(int, double)> sig;
	mcurses::slot<char(int, double)> slot_1 = [](int, double){return 'h';};
	auto to_track1 = std::make_shared<int>(7);
	auto to_track2 = std::make_shared<double>(8.3);
	slot_1.track(to_track1);
	slot_1.track(to_track2);
	mcurses::slot<char(int, double)> slot_2 = [](int, double){return 'r';};
	mcurses::slot<char(int, double)> slot_3 = [](int, double){return 'g';};

	sig.connect(slot_2);
	sig.connect(slot_1, mcurses::position::at_front);
	sig.connect(5, slot_3, mcurses::position::at_back);

	mcurses::signal<char(int, double)> sig_move_to = std::move(sig);

	auto result = sig_move_to(5, 4.3);

	EXPECT_TRUE(bool(result));
	EXPECT_EQ('r', *result);

	EXPECT_EQ(nullptr, sig.lock_impl());

	to_track1.reset();
	EXPECT_THROW(sig_move_to(4, 2.1), mcurses::expired_slot);
}

TEST(SignalTest, MoveAssignmentOperator)
{
	mcurses::signal<char(int, double)> sig;
	mcurses::slot<char(int, double)> slot_1 = [](int, double){return 'h';};
	auto to_track1 = std::make_shared<int>(7);
	auto to_track2 = std::make_shared<double>(8.3);
	slot_1.track(to_track1);
	slot_1.track(to_track2);
	mcurses::slot<char(int, double)> slot_2 = [](int, double){return 'r';};
	mcurses::slot<char(int, double)> slot_3 = [](int, double){return 'g';};

	sig.connect(slot_2);
	sig.connect(slot_1, mcurses::position::at_front);
	sig.connect(5, slot_3, mcurses::position::at_back);

	mcurses::signal<char(int, double)> sig_move_to;
	sig_move_to.connect([](int, double){return 'x';});
	sig_move_to = std::move(sig);

	auto result = sig_move_to(5, 4.3);

	EXPECT_TRUE(bool(result));
	EXPECT_EQ('r', *result);

	EXPECT_EQ(nullptr, sig.lock_impl());

	to_track1.reset();
	EXPECT_THROW(sig_move_to(4, 2.1), mcurses::expired_slot);
}

TEST(SignalTest, ConnectWithPosition)
{
	mcurses::signal<float()> sig;
	mcurses::slot<float()> slot_1 = [](){return 8.3;};
	auto conn1 = sig.connect(slot_1);

	mcurses::slot<float()> slot_2 = [](){return 1.2;};
	auto conn2 = sig.connect(slot_2, mcurses::position::at_front);

	mcurses::slot<float()> slot_3 = [](){return 3.4;};
	auto conn3 = sig.connect(slot_3, mcurses::position::at_back);

	auto result1 = sig();
	ASSERT_TRUE(bool(result1));
	EXPECT_FLOAT_EQ(3.4, *result1);

	conn3.disconnect();

	auto result2 = sig();
	ASSERT_TRUE(bool(result2));
	EXPECT_FLOAT_EQ(8.3, *result2);

	conn1.disconnect();

	auto result3 = sig();
	ASSERT_TRUE(bool(result3));
	EXPECT_FLOAT_EQ(1.2, *result3);

	conn2.disconnect();

	auto result4 = sig();
	EXPECT_FALSE(bool(result4));
}

TEST(SignalTest, ConnectWithGroup)
{
	mcurses::signal<char(int)> sig;
	mcurses::slot<char(int)> slot_1 = [](int){return 'a';};
	auto conn1 = sig.connect(1, slot_1);

	mcurses::slot<char(int)> slot_2 = [](int){return 'b';};
	auto conn2 = sig.connect(1, slot_2, mcurses::position::at_front);

	mcurses::slot<char(int)> slot_3 = [](int){return 'c';};
	auto conn3 = sig.connect(3, slot_3, mcurses::position::at_back);

	mcurses::slot<char(int)> slot_4 = [](int){return 'd';};
	auto conn4 = sig.connect(3, slot_4);

	mcurses::slot<char(int)> slot_5 = [](int){return 'e';};
	auto conn5 = sig.connect(100, slot_5);

	mcurses::slot<char(int)> slot_6 = [](int){return 'f';};
	auto conn6 = sig.connect(1, slot_6, mcurses::position::at_front);

	auto result1 = sig(1);
	ASSERT_TRUE(bool(result1));
	EXPECT_EQ('e', *result1);

	conn5.disconnect();

	auto result2 = sig(2);
	ASSERT_TRUE(bool(result2));
	EXPECT_EQ('d', *result2);

	conn4.disconnect();

	auto result3 = sig(3);
	ASSERT_TRUE(bool(result3));
	EXPECT_EQ('c', *result3);

	conn3.disconnect();

	auto result4 = sig(4);
	ASSERT_TRUE(bool(result4));
	EXPECT_EQ('a', *result4);

	conn1.disconnect();

	auto result5 = sig(5);
	ASSERT_TRUE(bool(result5));
	EXPECT_EQ('b', *result5);

	conn2.disconnect();

	auto result6 = sig(6);
	ASSERT_TRUE(bool(result6));
	EXPECT_EQ('f', *result6);

	conn6.disconnect();

	auto result7 = sig(7);
	EXPECT_FALSE(bool(result7));
}

TEST(SignalTest, ConnectBothOverloads)
{
	mcurses::signal<char(int)> sig;
	mcurses::slot<char(int)> slot_1 = [](int){return 'a';};
	auto conn1 = sig.connect(1, slot_1);

	mcurses::slot<char(int)> slot_2 = [](int){return 'b';};
	auto conn2 = sig.connect(1, slot_2, mcurses::position::at_front);

	mcurses::slot<char(int)> slot_3 = [](int){return 'c';};
	auto conn3 = sig.connect(3, slot_3, mcurses::position::at_back);

	mcurses::slot<char(int)> slot_4 = [](int){return 'd';};
	auto conn4 = sig.connect(3, slot_4);

	mcurses::slot<char(int)> slot_5 = [](int){return 'e';};
	auto conn5 = sig.connect(100, slot_5);

	mcurses::slot<char(int)> slot_6 = [](int){return 'f';};
	auto conn6 = sig.connect(1, slot_6, mcurses::position::at_front);

	mcurses::slot<char(int)> slot_7 = [](int){return 'g';};
	auto conn7 = sig.connect(slot_7, mcurses::position::at_front);

	mcurses::slot<char(int)> slot_8 = [](int){return 'h';};
	auto conn8 = sig.connect(slot_8, mcurses::position::at_front);

	mcurses::slot<char(int)> slot_9 = [](int){return 'i';};
	auto conn9 = sig.connect(slot_9, mcurses::position::at_back);


	auto result0 = sig(0);
	ASSERT_TRUE(bool(result0));
	EXPECT_EQ('i', *result0);

	conn9.disconnect();

	auto result1 = sig(1);
	ASSERT_TRUE(bool(result1));
	EXPECT_EQ('e', *result1);

	conn5.disconnect();

	auto result2 = sig(2);
	ASSERT_TRUE(bool(result2));
	EXPECT_EQ('d', *result2);

	conn4.disconnect();

	auto result3 = sig(3);
	ASSERT_TRUE(bool(result3));
	EXPECT_EQ('c', *result3);

	conn3.disconnect();

	auto result4 = sig(4);
	ASSERT_TRUE(bool(result4));
	EXPECT_EQ('a', *result4);

	conn1.disconnect();

	auto result5 = sig(5);
	ASSERT_TRUE(bool(result5));
	EXPECT_EQ('b', *result5);

	conn2.disconnect();

	auto result6 = sig(6);
	ASSERT_TRUE(bool(result6));
	EXPECT_EQ('f', *result6);

	conn6.disconnect();

	auto result7 = sig(7);
	ASSERT_TRUE(bool(result7));
	EXPECT_EQ('g', *result7);

	conn7.disconnect();

	auto result8 = sig(8);
	ASSERT_TRUE(bool(result8));
	EXPECT_EQ('h', *result8);

	conn8.disconnect();

	auto result9 = sig(9);
	EXPECT_FALSE(bool(result9));
}

TEST(SignalTest, ConnectWithTrackedObject)
{
	mcurses::signal<int(char, char)> sig;
	mcurses::slot<int(char, char)> slot = [](char, char){return 3;};
	auto track_me = std::make_shared<int>(9);
	slot.track(track_me);

	auto conn = sig.connect(slot);

	auto result = sig('l', 'k');
	ASSERT_TRUE(bool(result));
	EXPECT_EQ(3, *result);

	track_me.reset();
	EXPECT_THROW(sig('l', 'k'), mcurses::expired_slot); // slot has expired
}

TEST(SignalTest, ConnectExtendedWithPosition)
{
	using mcurses::connection;
	mcurses::signal<char(int, int)> sig;

	mcurses::slot<char(const connection&, int, int)> slot_1 = [](const connection&, int, int){return 'a';};
	auto conn1 = sig.connect_extended(slot_1);

	mcurses::slot<char(const connection&, int, int)> slot_2 = [](const connection&, int, int){return 'b';};
	auto conn2 = sig.connect_extended(slot_2, mcurses::position::at_front);

	mcurses::slot<char(const connection&, int, int)> slot_3 = [](const connection&, int, int){return 'c';};
	auto conn3 = sig.connect_extended(slot_3, mcurses::position::at_front);

	mcurses::slot<char(const connection&, int, int)> slot_4 = [](const connection&, int, int){return 'd';};
	auto conn4 = sig.connect_extended(slot_4, mcurses::position::at_back);

	mcurses::slot<char(const connection&, int, int)> slot_5 = [](const connection&, int, int){return 'e';};
	auto conn5 = sig.connect_extended(slot_5);

	auto result1 = sig(5,4);
	ASSERT_TRUE(bool(result1));
	EXPECT_EQ('e', *result1);
	conn5.disconnect();

	auto result2 = sig(5,4);
	ASSERT_TRUE(bool(result2));
	EXPECT_EQ('d', *result2);
	conn4.disconnect();

	auto result3 = sig(5,4);
	ASSERT_TRUE(bool(result3));
	EXPECT_EQ('a', *result3);
	conn1.disconnect();

	auto result4 = sig(5,4);
	ASSERT_TRUE(bool(result4));
	EXPECT_EQ('b', *result4);
	conn2.disconnect();

	auto result5 = sig(5,4);
	ASSERT_TRUE(bool(result5));
	EXPECT_EQ('c', *result5);
	conn3.disconnect();

	auto result6 = sig(5,4);
	EXPECT_FALSE(bool(result6));
}

TEST(SignalTest, ConnectExtendedWithGroup)
{
	using mcurses::connection;
	mcurses::signal<char(int)> sig;
	mcurses::slot<char(const connection&, int)> slot_1 = [](const connection&, int){return 'a';};
	auto conn1 = sig.connect_extended(1, slot_1);

	mcurses::slot<char(const connection&, int)> slot_2 = [](const connection&, int){return 'b';};
	auto conn2 = sig.connect_extended(1, slot_2, mcurses::position::at_front);

	mcurses::slot<char(const connection&, int)> slot_3 = [](const connection&, int){return 'c';};
	auto conn3 = sig.connect_extended(3, slot_3, mcurses::position::at_back);

	mcurses::slot<char(const connection&, int)> slot_4 = [](const connection&, int){return 'd';};
	auto conn4 = sig.connect_extended(3, slot_4);

	mcurses::slot<char(const connection&, int)> slot_5 = [](const connection&, int){return 'e';};
	auto conn5 = sig.connect_extended(100, slot_5);

	mcurses::slot<char(const connection&, int)> slot_6 = [](const connection&, int){return 'f';};
	auto conn6 = sig.connect_extended(1, slot_6, mcurses::position::at_front);

	auto result1 = sig(1);
	ASSERT_TRUE(bool(result1));
	EXPECT_EQ('e', *result1);

	conn5.disconnect();

	auto result2 = sig(2);
	ASSERT_TRUE(bool(result2));
	EXPECT_EQ('d', *result2);

	conn4.disconnect();

	auto result3 = sig(3);
	ASSERT_TRUE(bool(result3));
	EXPECT_EQ('c', *result3);

	conn3.disconnect();

	auto result4 = sig(4);
	ASSERT_TRUE(bool(result4));
	EXPECT_EQ('a', *result4);

	conn1.disconnect();

	auto result5 = sig(5);
	ASSERT_TRUE(bool(result5));
	EXPECT_EQ('b', *result5);

	conn2.disconnect();

	auto result6 = sig(6);
	ASSERT_TRUE(bool(result6));
	EXPECT_EQ('f', *result6);

	conn6.disconnect();

	auto result7 = sig(7);
	EXPECT_FALSE(bool(result7));
}

TEST(SignalTest, ConnectExtendedWithBothOverloads)
{
	using mcurses::connection;
	mcurses::signal<char(int)> sig;
	mcurses::slot<char(const connection&, int)> slot_1 = [](const connection&, int){return 'a';};
	auto conn1 = sig.connect_extended(1, slot_1);

	mcurses::slot<char(const connection&, int)> slot_2 = [](const connection&, int){return 'b';};
	auto conn2 = sig.connect_extended(1, slot_2, mcurses::position::at_front);

	mcurses::slot<char(const connection&, int)> slot_3 = [](const connection&, int){return 'c';};
	auto conn3 = sig.connect_extended(3, slot_3, mcurses::position::at_back);

	mcurses::slot<char(const connection&, int)> slot_4 = [](const connection&, int){return 'd';};
	auto conn4 = sig.connect_extended(3, slot_4);

	mcurses::slot<char(const connection&, int)> slot_5 = [](const connection&, int){return 'e';};
	auto conn5 = sig.connect_extended(100, slot_5);

	mcurses::slot<char(const connection&, int)> slot_6 = [](const connection&, int){return 'f';};
	auto conn6 = sig.connect_extended(1, slot_6, mcurses::position::at_front);

	mcurses::slot<char(const connection&, int)> slot_7 = [](const connection&, int){return 'g';};
	auto conn7 = sig.connect_extended(slot_7, mcurses::position::at_front);

	mcurses::slot<char(const connection&, int)> slot_8 = [](const connection&, int){return 'h';};
	auto conn8 = sig.connect_extended(slot_8, mcurses::position::at_front);

	mcurses::slot<char(const connection&, int)> slot_9 = [](const connection&, int){return 'i';};
	auto conn9 = sig.connect_extended(slot_9, mcurses::position::at_back);


	auto result0 = sig(0);
	ASSERT_TRUE(bool(result0));
	EXPECT_EQ('i', *result0);

	conn9.disconnect();

	auto result1 = sig(1);
	ASSERT_TRUE(bool(result1));
	EXPECT_EQ('e', *result1);

	conn5.disconnect();

	auto result2 = sig(2);
	ASSERT_TRUE(bool(result2));
	EXPECT_EQ('d', *result2);

	conn4.disconnect();

	auto result3 = sig(3);
	ASSERT_TRUE(bool(result3));
	EXPECT_EQ('c', *result3);

	conn3.disconnect();

	auto result4 = sig(4);
	ASSERT_TRUE(bool(result4));
	EXPECT_EQ('a', *result4);

	conn1.disconnect();

	auto result5 = sig(5);
	ASSERT_TRUE(bool(result5));
	EXPECT_EQ('b', *result5);

	conn2.disconnect();

	auto result6 = sig(6);
	ASSERT_TRUE(bool(result6));
	EXPECT_EQ('f', *result6);

	conn6.disconnect();

	auto result7 = sig(7);
	ASSERT_TRUE(bool(result7));
	EXPECT_EQ('g', *result7);

	conn7.disconnect();

	auto result8 = sig(8);
	ASSERT_TRUE(bool(result8));
	EXPECT_EQ('h', *result8);

	conn8.disconnect();

	auto result9 = sig(9);
	EXPECT_FALSE(bool(result9));
}

TEST(SignalTest, ConnectExtendedSlotWithTrackedObject)
{
	mcurses::signal<int(char, char)> sig;
	mcurses::slot<int(const mcurses::connection&, char, char)>
		ext_slot = [](const mcurses::connection&, char, char){return 5;};
	auto track_me = std::make_shared<int>(2);
	ext_slot.track(track_me);

	auto conn = sig.connect_extended(ext_slot);

	EXPECT_TRUE(conn.connected());
	auto result = sig('h', 'k');
	ASSERT_TRUE(bool(result));
	EXPECT_EQ(5, *result);

	track_me.reset();
	EXPECT_EQ(0, track_me.use_count());

	EXPECT_THROW(ext_slot(conn, 'j', 'o'), mcurses::expired_slot);
	EXPECT_THROW(sig('l', 'k'), mcurses::expired_slot);
}

TEST(SignalTest, DisconnectByGroup)
{
	mcurses::signal<char(int)> sig;
	
	mcurses::slot<char(int)> slot_1 = [](int){return 'a';};
	sig.connect(1, slot_1, mcurses::position::at_front);

	mcurses::slot<char(int)> slot_2 = [](int){return 'b';};
	sig.connect(2, slot_2);

	mcurses::slot<char(int)> slot_3 = [](int){return 'c';};
	sig.connect(2, slot_3, mcurses::position::at_front);

	mcurses::slot<char(int)> slot_4 = [](int){return 'd';};
	sig.connect(3, slot_4);
	
	auto result1 = sig(4);
	ASSERT_TRUE(bool(result1));
	EXPECT_EQ('d', *result1);

	sig.disconnect(3);

	auto result2 = sig(4);
	ASSERT_TRUE(bool(result2));
	EXPECT_EQ('b', *result2);

	sig.disconnect(2);

	auto result3 = sig(4);
	ASSERT_TRUE(bool(result3));
	EXPECT_EQ('a', *result3);
}

TEST(SignalTest, DisconnectAllSlots)
{
	mcurses::signal<char(char, int)> sig;

	mcurses::slot<char(char, int)> slot_1 = [](char, int){return 'a';};
	mcurses::slot<char(const mcurses::connection&, char, int)> slot_2 = [](const mcurses::connection&, char, int){return 'b';};
	mcurses::slot<char(char, int)> slot_3 = [](char, int){return 'c';};

	sig.connect(slot_1);
	sig.connect_extended(4, slot_2);
	sig.connect(slot_3, mcurses::position::at_front);

	auto result1 = sig('g', 6);
	ASSERT_TRUE(bool(result1));

	sig.disconnect_all_slots();

	auto result2 = sig('d', 3);
	EXPECT_FALSE(bool(result2));
}

TEST(SignalTest, Empty)
{
	mcurses::signal<int(char)> sig;

	mcurses::slot<int(char)> slot_1 = [](char){return 1;};
	sig.connect(slot_1);

	EXPECT_FALSE(sig.empty());

	sig.disconnect_all_slots();
	EXPECT_TRUE(sig.empty());

	mcurses::slot<int(const mcurses::connection&, char)> slot_2 = [](const mcurses::connection&, char){return 2;};
	sig.connect_extended(slot_2);

	EXPECT_FALSE(sig.empty());

	sig.disconnect_all_slots();

	EXPECT_TRUE(sig.empty());
}

TEST(SignalTest, NumSlots)
{
	mcurses::signal<int()> sig;

	mcurses::slot<int()> slot_1 = [](){return 1;};
	sig.connect(slot_1);
	mcurses::slot<int()> slot_2 = [](){return 2;};
	sig.connect(slot_2);
	mcurses::slot<int()> slot_3 = [](){return 3;};
	sig.connect(2, slot_3);
	mcurses::slot<int()> slot_4 = [](){return 4;};
	sig.connect(2, slot_4);

	EXPECT_EQ(4, sig.num_slots());

	mcurses::slot<int()> slot_5 = [](){return 5;};
	sig.connect(3, slot_5, mcurses::position::at_back);
	mcurses::slot<int(const mcurses::connection&)> slot_6 = [](const mcurses::connection&){return 6;};
	sig.connect_extended(slot_6);
	mcurses::slot<int()> slot_7 = [](){return 7;};
	sig.connect(slot_7);

	EXPECT_EQ(7, sig.num_slots());

	sig.disconnect(2);

	EXPECT_EQ(5, sig.num_slots());
}

TEST(SignalTest, OperatorParenthesis)
{
	mcurses::signal<void(char)> sig1;
	sig1.connect([](char){return;});

	sig1('d'); // returns void

	mcurses::signal<int(double)> sig2;
	sig2.connect([](double){return 4;});
	EXPECT_TRUE(bool(sig2(4.3)));
	EXPECT_EQ(4, *sig2(4.5));

}

TEST(SignalTest, ConstOperatorParenthesis)
{
	mcurses::signal<void(char)> sig1;
	sig1.connect([](char){return;});

	const mcurses::signal<void(char)> sig1_const = std::move(sig1);

	sig1_const('d'); // returns void

	mcurses::signal<int(double)> sig2;
	sig2.connect([](double){return 4;});
	const mcurses::signal<int(double)> sig2_const = std::move(sig2);

	EXPECT_TRUE(bool(sig2_const(4.3)));
	EXPECT_EQ(4, *sig2_const(4.5));
}

template <typename return_type>
class new_combiner{
public:
	new_combiner(){};
	new_combiner(int val):value_{val}{}
	typedef return_type result_type;
	template <typename InputIterator>
	return_type operator()(InputIterator first, InputIterator last)
	{
		return return_type();
	}
	int get_value(){return value_;}
private:
	int value_ = 0;
};

TEST(SignalTest, Combiner)
{
	mcurses::signal<void(int)> sig1;
	EXPECT_EQ(typeid(mcurses::optional_last_value<void>{}), typeid(sig1.combiner()));

	mcurses::signal<int(double), mcurses::optional_last_value<int>> sig2;
	EXPECT_EQ(typeid(mcurses::optional_last_value<int>{}), typeid(sig2.combiner()));
	
	mcurses::signal<int(double), new_combiner<int>> sig3;
	EXPECT_EQ(typeid(new_combiner<int>{}), typeid(sig3.combiner()));
}

TEST(SignalTest, SetCombiner)
{
	mcurses::signal<int(double, char)> sig1;
	sig1.set_combiner(mcurses::optional_last_value<int>{});
	EXPECT_EQ(typeid(mcurses::optional_last_value<int>{}), typeid(sig1.combiner()));

	mcurses::signal<int(double, char), new_combiner<int>> sig2;
	sig2.set_combiner(new_combiner<int>{});
	EXPECT_EQ(typeid(new_combiner<int>{}), typeid(sig2.combiner()));

	mcurses::signal<int(double, char), new_combiner<int>> sig3;
	sig3.set_combiner(new_combiner<int>{7});
	EXPECT_EQ(7, sig3.combiner().get_value());
	sig3.set_combiner(new_combiner<int>{3});
	EXPECT_EQ(3, sig3.combiner().get_value());
}

TEST(SignalTest, Swap)
{
	mcurses::signal<int(char)> sig1;
	mcurses::signal<int(char)> sig2;

	mcurses::slot<int(char)> slot_1 = [](char){return 1;};
	sig1.connect(slot_1);
	sig2.connect(slot_1);
	mcurses::slot<int(char)> slot_2 = [](char){return 2;};
	sig1.connect(slot_2);
	sig2.connect(slot_2);
	mcurses::slot<int(char)> slot_3 = [](char){return 3;};
	sig1.connect(slot_3);
	sig2.connect(slot_3);
	mcurses::slot<int(char)> slot_4 = [](char){return 4;};
	sig2.connect(slot_4);
	mcurses::slot<int(char)> slot_5 = [](char){return 5;};
	sig2.connect(slot_5);
	mcurses::slot<int(char)> slot_6 = [](char){return 6;};
	sig2.connect(slot_6);

	auto result1 = sig1('d');
	auto result2 = sig2('g');

	ASSERT_TRUE(bool(result1));
	EXPECT_EQ(3, *result1);

	ASSERT_TRUE(bool(result2));
	EXPECT_EQ(6, *result2);

	swap(sig1, sig2);

	result1 = sig1('d');
	result2 = sig2('g');

	ASSERT_TRUE(bool(result1));
	EXPECT_EQ(6, *result1);

	ASSERT_TRUE(bool(result2));
	EXPECT_EQ(3, *result2);

	swap(sig2, sig1);

	result1 = sig1('d');
	result2 = sig2('g');

	ASSERT_TRUE(bool(result1));
	EXPECT_EQ(3, *result1);

	ASSERT_TRUE(bool(result2));
	EXPECT_EQ(6, *result2);
}

TEST(SignalTest, LockImplAsVoid)
{
	mcurses::signal<int(char)> sig;
	mcurses::slot<int(char)> slot_1 = [](char){return 6;};
	sig.connect(slot_1);

	EXPECT_EQ(2, sig.lock_impl().use_count());
}

TEST(SignalTest, LockImpl)
{
	mcurses::signal<int(char)> sig;
	mcurses::slot<int(char)> slot_1 = [](char){return 6;};
	sig.connect(slot_1);

	EXPECT_EQ(1, sig.lock_impl()->num_slots());
	EXPECT_EQ(2, sig.lock_impl().use_count());
}