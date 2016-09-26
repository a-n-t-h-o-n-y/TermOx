#include <mcurses/signal_module/detail/signal_impl.hpp>
#include <gtest/gtest.h>

#include <mcurses/signal_module/signal.hpp>
#include <mcurses/signal_module/optional_last_value.hpp>
#include <mcurses/signal_module/connection.hpp>
#include <mcurses/signal_module/position.hpp>
#include <functional>
#include <boost/function.hpp>
#include <type_traits>
#include <typeinfo>
#include <vector>

class SignalImplTest: public testing::Test{
protected:
	// type_1 - void(int), less<int>, std::function
	typedef mcurses::signal_impl<void(int), mcurses::optional_last_value<void>,
				int, std::less<int>, std::function<void(int)>> type_1;

	// type_2 - double(char, int), less<char>, std::function
	typedef mcurses::signal_impl<double(char, int), mcurses::optional_last_value<double>,
				char, std::less<char>, std::function<double(char, int)>> type_2;

	// type_3 - unsigned(long long), less<double>, boost::function
	typedef mcurses::signal_impl<unsigned(long long), mcurses::optional_last_value<unsigned>,
				double, std::greater<double>, boost::function<unsigned(long long)>> type_3;

	// type_4 - char(int, double), less<int>, std::function
	typedef mcurses::signal_impl<char(int, double), mcurses::optional_last_value<char>,
				int, std::less<int>, std::function<char(int, double)>> type_4;

	SignalImplTest()
	{
		signal_std.connect(boost_slot_non_empty1);
		signal_std.connect(std_slot_non_empty1);
		std_slot_holds_signal = mcurses::slot<void(int)>{signal_std};

		signal_boost.connect(boost_slot_empty2);
		signal_boost.connect(std_slot_empty2);
		boost_slot_holds_signal = mcurses::slot<mcurses::optional<unsigned>(long long),
					boost::function<mcurses::optional<unsigned>(long long)>>{signal_boost};

		si_type1_2.connect([](int){return;}, mcurses::position::at_back);

		si_type2_2.connect([](char, int){return 0.3;}, mcurses::position::at_back);
		si_type2_2.connect([](char, int){return 7.3;}, mcurses::position::at_back);
		si_type2_2.connect([](char, int){return 9.1;}, mcurses::position::at_back);

		si_type3_2.connect([](long long){return 4;}, mcurses::position::at_back);
		si_type3_2.connect([](long long){return 5;}, mcurses::position::at_back);
		si_type3_2.connect([](long long){return 6;}, mcurses::position::at_back);
		si_type3_2.connect([](long long){return 7;}, mcurses::position::at_back);
		si_type3_2.connect([](long long){return 8;}, mcurses::position::at_back);
		si_type3_2.connect([](long long){return 9;}, mcurses::position::at_back);
	}

	// type_1 - void(int), less<int>, std::function
	type_1 si_type1_1{mcurses::optional_last_value<void>(), std::less<int>()};
	type_1 si_type1_2{mcurses::optional_last_value<void>(), std::less<int>()};

	// type_2 - double(char, int), less<char>, std::function
	type_2 si_type2_1{mcurses::optional_last_value<double>(), std::less<char>()};
	type_2 si_type2_2{mcurses::optional_last_value<double>(), std::less<char>()};

	// type_3 - unsigned(long long), greater<double>, boost::function
	type_3 si_type3_1{mcurses::optional_last_value<unsigned>(), std::greater<double>()};
	type_3 si_type3_2{mcurses::optional_last_value<unsigned>(), std::greater<double>()};

	// type 4 - char(int, double), less<int>, std::function
	type_4 si_type4{mcurses::optional_last_value<char>(), std::less<int>()};

	// Slots - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Empty Slot - boost::function
	mcurses::slot<void(int), boost::function<void(int)>> boost_slot_empty1{};

	mcurses::slot<void(int), boost::function<void(int)>> boost_slot_non_empty1{[](int){return;}};

	// Empty Slot - boost::function
	mcurses::slot<unsigned(long long), boost::function<unsigned(long long)>> boost_slot_empty2{};

	mcurses::slot<unsigned(long long), boost::function<unsigned(long long)>> boost_slot_non_empty2{[](long long){return 3;}};

	// Function Slot - boost::function
	mcurses::slot<void(unsigned), boost::function<void(unsigned)>> boost_slot_function1{[](unsigned){return;}};

	// Function Slot - boost::function
	mcurses::slot<double(char), boost::function<double(char)>> boost_slot_function2{[](char){return 3.4;}};

	// Extended Slot - boost::function
	mcurses::slot<char(const mcurses::connection&, int, double), boost::function<char(const mcurses::connection&, int, double)>>
			boost_extended_slot{[](const mcurses::connection&, int, double){return 'h';}};

	// Slot holds empty Signal - boost::function
	mcurses::signal<unsigned(long long), mcurses::optional_last_value<unsigned>,
					int, std::less<int>, boost::function<unsigned(long long)>> empty_signal_boost{};
	mcurses::slot<mcurses::optional<unsigned>(long long), boost::function<mcurses::optional<unsigned>(long long)>>
					boost_slot_holds_empty_signal{empty_signal_boost};

	// Slot holds non-empty Signal - boost::function
	mcurses::signal<unsigned(long long), mcurses::optional_last_value<unsigned>,
					int, std::less<int>, boost::function<unsigned(long long)>> signal_boost{};
	mcurses::slot<mcurses::optional<unsigned>(long long), boost::function<mcurses::optional<unsigned>(long long)>>
					boost_slot_holds_signal{};

	// Empty Slot - std::function
	mcurses::slot<void(int)> std_slot_empty1{};

	mcurses::slot<void(int)> std_slot_non_empty1{[](int){return;}};

	// Empty Slot - std::function
	mcurses::slot<unsigned(long long)> std_slot_empty2{};

	mcurses::slot<unsigned(long long)> std_slot_non_empty2{[](long long){return 7;}};

	// Function Slot - std::function
	mcurses::slot<void(unsigned)> std_slot_function1{[](unsigned){return;}};

	// Function Slot - std::function
	mcurses::slot<double(char)> std_slot_function2{[](char){return 3.4;}};

	// Extended Slot - std::function
	mcurses::slot<char(const mcurses::connection&, int, double)> std_extended_slot{[](const mcurses::connection&, int, double){return 'k';}};

	// Slot holds empty Signal - std::function
	mcurses::signal<void(int)> empty_signal_std{};
	mcurses::slot<void(int)> std_slot_holds_empty_signal{empty_signal_std};

	// Slot holds non-empty Signal - std::function
	mcurses::signal<void(int)> signal_std{};
	mcurses::slot<void(int)> std_slot_holds_signal{};

	// Functors - - - - - - - - - - - - - - - - - - - - -
	class functor {
	public:
		double operator()(char, int)
		{
			return 6.5;
		}
	};
};

TEST_F(SignalImplTest, Constructor)
{
	EXPECT_EQ(0, si_type1_1.num_slots());
	EXPECT_EQ(0, si_type2_1.num_slots());
	EXPECT_EQ(0, si_type3_1.num_slots());

	EXPECT_TRUE(si_type1_1.empty());
	EXPECT_TRUE(si_type2_1.empty());
	EXPECT_TRUE(si_type3_1.empty());

	si_type1_1(7);	// returns void
	EXPECT_FALSE(bool(si_type2_1('l', 5)));
	EXPECT_FALSE(bool(si_type3_1(99)));
}

TEST_F(SignalImplTest, CopyConstructor)
{
	type_1 copied = si_type1_1;
	EXPECT_EQ(0, copied.num_slots());
	EXPECT_TRUE(copied.empty());
	copied(7);	// returns void

	type_2 copied2 = si_type2_2;
	EXPECT_EQ(3, copied2.num_slots());
	EXPECT_FALSE(copied2.empty());
	ASSERT_TRUE(bool(copied2('f', 7)));
	EXPECT_DOUBLE_EQ(9.1, *(copied2('4', 6)));

	type_3 copied3 = si_type3_2;
	EXPECT_EQ(6, copied3.num_slots());
	EXPECT_FALSE(copied3.empty());
	ASSERT_TRUE(bool(copied3(9999)));
	EXPECT_EQ(9, *(copied3(9997876)));
}

TEST_F(SignalImplTest, MoveConstructor)
{
	type_1 sig_impl_mv1{std::move(si_type1_1)};
	EXPECT_TRUE(si_type1_1.empty());

	type_1 sig_impl_mv2{std::move(si_type1_2)};
	EXPECT_TRUE(si_type1_2.empty());
	EXPECT_EQ(1, sig_impl_mv2.num_slots());

	type_3 sig_impl_mv3{std::move(si_type3_2)};
	EXPECT_TRUE(si_type3_2.empty());
	EXPECT_EQ(6, sig_impl_mv3.num_slots());
}

TEST_F(SignalImplTest, CopyAssignmentOperator)
{
	EXPECT_TRUE(si_type1_1.empty());
	si_type1_1 = si_type1_2;
	EXPECT_EQ(1, si_type1_1.num_slots());
	EXPECT_EQ(1, si_type1_2.num_slots());

	EXPECT_EQ(6, si_type3_2.num_slots());
	si_type3_1 = si_type3_2;
	EXPECT_EQ(6, si_type3_1.num_slots());
	EXPECT_EQ(6, si_type3_2.num_slots());
}

TEST_F(SignalImplTest, MoveAssignmentOperator)
{
	EXPECT_TRUE(si_type1_1.empty());
	si_type1_1 = std::move(si_type1_2);
	EXPECT_EQ(1, si_type1_1.num_slots());
	EXPECT_TRUE(si_type1_2.empty());

	EXPECT_EQ(6, si_type3_2.num_slots());
	si_type3_1 = std::move(si_type3_2);
	EXPECT_EQ(6, si_type3_1.num_slots());
	EXPECT_TRUE(si_type3_2.empty());
}

TEST_F(SignalImplTest, ConnectPosition)
{
	// void(int)
	// this slot calls a signal which calls both a std slot and a boost slot.
	mcurses::connection conn1 = si_type1_1.connect(std_slot_holds_signal, mcurses::position::at_back);
	mcurses::connection conn2 = si_type1_1.connect(std_slot_non_empty1, mcurses::position::at_front);
	EXPECT_EQ(2, si_type1_1.num_slots());
	si_type1_1(7);	// returns void

	// double(char, int)
	mcurses::connection conn3 = si_type2_1.connect([](char, int){return 8.3;}, mcurses::position::at_back);
	mcurses::connection conn4 = si_type2_1.connect([](char, int){return 2.8;}, mcurses::position::at_front);
	EXPECT_EQ(2, si_type2_1.num_slots());
	EXPECT_DOUBLE_EQ(8.3, *si_type2_1('f', 5));
	conn3.disconnect();
	EXPECT_EQ(1, si_type2_1.num_slots());
	EXPECT_DOUBLE_EQ(2.8, *si_type2_1('k', 8));

	// unsigned(long long)
	mcurses::connection conn5 = si_type3_1.connect(std_slot_non_empty2, mcurses::position::at_back); // returns 7
	mcurses::connection conn6 = si_type3_1.connect(boost_slot_non_empty2, mcurses::position::at_back); // returns 3
	mcurses::connection conn7 = si_type3_1.connect([](long long){return 2;}, mcurses::position::at_front);

	EXPECT_EQ(3, si_type3_1.num_slots());
	EXPECT_EQ(3, *si_type3_1(9999999));
	conn6.disconnect();
	EXPECT_EQ(7, *si_type3_1(7654321));
	conn5.disconnect();
	EXPECT_EQ(2, *si_type3_1(1234567));
}

TEST_F(SignalImplTest, ConnectGroupPosition)
{
	// void(int), less<int>
	mcurses::connection conn1 = si_type1_1.connect(3, std_slot_holds_signal, mcurses::position::at_back);
	mcurses::connection conn2 = si_type1_1.connect(1, std_slot_non_empty1, mcurses::position::at_front);
	EXPECT_EQ(2, si_type1_1.num_slots());
	si_type1_1(7);	// returns void

	// double(char, int), less<char>
	mcurses::connection conn3 = si_type2_1.connect('a', [](char, int){return 8.3;}, mcurses::position::at_back);
	mcurses::connection conn4 = si_type2_1.connect('z', [](char, int){return 2.8;}, mcurses::position::at_front);
	mcurses::connection conn5 = si_type2_1.connect('z', [](char, int){return 7.3;}, mcurses::position::at_back);
	EXPECT_EQ(3, si_type2_1.num_slots());
	EXPECT_DOUBLE_EQ(7.3, *si_type2_1('f', 5));
	conn5.disconnect();
	EXPECT_EQ(2, si_type2_1.num_slots());
	EXPECT_DOUBLE_EQ(2.8, *si_type2_1('k', 8));
	conn4.disconnect();
	EXPECT_EQ(1, si_type2_1.num_slots());
	EXPECT_DOUBLE_EQ(8.3, *si_type2_1('k', 8));

	// unsigned(long long), greater<double>
	mcurses::connection conn6 = si_type3_1.connect(5.4, std_slot_non_empty2, mcurses::position::at_back); // returns 7
	mcurses::connection conn7 = si_type3_1.connect(5.3, boost_slot_non_empty2, mcurses::position::at_back); // returns 3
	mcurses::connection conn8 = si_type3_1.connect(5.3, [](long long){return 8;}, mcurses::position::at_front);
	mcurses::connection conn9 = si_type3_1.connect(0.77, [](long long){return 2;}, mcurses::position::at_front);

	EXPECT_EQ(4, si_type3_1.num_slots());
	EXPECT_EQ(2, *si_type3_1(9999999));
	conn9.disconnect();
	EXPECT_EQ(3, *si_type3_1(7654321));
	conn7.disconnect();
	EXPECT_EQ(8, *si_type3_1(1234567));
	conn8.disconnect();
	EXPECT_EQ(7, *si_type3_1(1234567));
}

TEST_F(SignalImplTest, BothConnects)
{
	// double(char, int), less<char>
	mcurses::connection conn1 = si_type2_1.connect('a', [](char, int){return 8.3;}, mcurses::position::at_back);
	mcurses::connection conn2 = si_type2_1.connect([](char, int){return 2.8;}, mcurses::position::at_front);
	mcurses::connection conn3 = si_type2_1.connect([](char, int){return 9.1;}, mcurses::position::at_back);
	mcurses::connection conn4 = si_type2_1.connect('z', [](char, int){return 7.3;}, mcurses::position::at_front);

	EXPECT_DOUBLE_EQ(9.1, *si_type2_1('f', 5));
	conn3.disconnect();

	EXPECT_DOUBLE_EQ(7.3, *si_type2_1('f', 5));
	conn4.disconnect();

	EXPECT_DOUBLE_EQ(8.3, *si_type2_1('k', 8));
	conn1.disconnect();

	EXPECT_DOUBLE_EQ(2.8, *si_type2_1('k', 8));
}

TEST_F(SignalImplTest, ConnectExtendedPosition)
{
	// char(int, double)
	mcurses::connection conn1 = si_type4.connect_extended(boost_extended_slot, mcurses::position::at_front);
	EXPECT_FALSE(conn1 == mcurses::connection());
	EXPECT_EQ(1, si_type4.num_slots());
	ASSERT_TRUE(bool(si_type4(6, 3.42)));
	EXPECT_EQ('h', *si_type4(6, 3.42));

	mcurses::connection conn2 = si_type4.connect_extended(std_extended_slot, mcurses::position::at_back);
	EXPECT_EQ(2, si_type4.num_slots());
	ASSERT_TRUE(bool(si_type4(8, 0.43)));
	EXPECT_EQ('k', *si_type4(8, 0.43));

	conn1.disconnect();

	EXPECT_EQ(1, si_type4.num_slots());
	ASSERT_TRUE(bool(si_type4(8, 0.43)));
	EXPECT_EQ('k', *si_type4(8, 0.43));
}

TEST_F(SignalImplTest, ConnectExtendedGroupPosition)
{
	// char(int, double)
	mcurses::connection conn1 = si_type4.connect_extended(5, boost_extended_slot, mcurses::position::at_front);
	EXPECT_FALSE(conn1 == mcurses::connection());
	EXPECT_EQ(1, si_type4.num_slots());
	ASSERT_TRUE(bool(si_type4(6, 3.42)));
	EXPECT_EQ('h', *si_type4(6, 3.42));

	mcurses::connection conn2 = si_type4.connect_extended(3, std_extended_slot, mcurses::position::at_back);
	EXPECT_EQ(2, si_type4.num_slots());
	ASSERT_TRUE(bool(si_type4(8, 0.43)));
	EXPECT_EQ('h', *si_type4(8, 0.43));

	conn1.disconnect();

	EXPECT_EQ(1, si_type4.num_slots());
	ASSERT_TRUE(bool(si_type4(8, 0.43)));
	EXPECT_EQ('k', *si_type4(8, 0.43));
}

TEST_F(SignalImplTest, ConnectExtendedBoth)
{
	// char(int, double)
	mcurses::connection conn1 = si_type4.connect_extended(5, boost_extended_slot, mcurses::position::at_front);
	EXPECT_FALSE(conn1 == mcurses::connection());
	EXPECT_EQ(1, si_type4.num_slots());
	ASSERT_TRUE(bool(si_type4(6, 3.42)));
	EXPECT_EQ('h', *si_type4(6, 3.42));

	mcurses::connection conn2 = si_type4.connect_extended([](const mcurses::connection&, int, double){return 'l';}, mcurses::position::at_front);
	EXPECT_EQ(2, si_type4.num_slots());
	ASSERT_TRUE(bool(si_type4(8, 0.43)));
	EXPECT_EQ('h', *si_type4(8, 0.43));

	mcurses::connection conn3 = si_type4.connect_extended([](const mcurses::connection&, int, double){return 'p';}, mcurses::position::at_back);
	EXPECT_EQ(3, si_type4.num_slots());
	ASSERT_TRUE(bool(si_type4(8, 0.43)));
	EXPECT_EQ('p', *si_type4(8, 0.43));

	mcurses::connection conn4 = si_type4.connect_extended(3, std_extended_slot, mcurses::position::at_back);
	EXPECT_EQ(4, si_type4.num_slots());
	ASSERT_TRUE(bool(si_type4(8, 0.43)));
	EXPECT_EQ('p', *si_type4(8, 0.43));

	conn3.disconnect();
	EXPECT_EQ(3, si_type4.num_slots());
	ASSERT_TRUE(bool(si_type4(8, 0.43)));
	EXPECT_EQ('h', *si_type4(8, 0.43));

	conn1.disconnect();
	EXPECT_EQ(2, si_type4.num_slots());
	ASSERT_TRUE(bool(si_type4(8, 0.43)));
	EXPECT_EQ('k', *si_type4(8, 0.43));

	conn4.disconnect();
	EXPECT_EQ(1, si_type4.num_slots());
	ASSERT_TRUE(bool(si_type4(8, 0.43)));
	EXPECT_EQ('l', *si_type4(8, 0.43));

	conn2.disconnect();
	EXPECT_TRUE(si_type4.empty());
	EXPECT_FALSE(bool(si_type4(8, 0.43)));
}

TEST_F(SignalImplTest, DisconnectByGroup)
{
	// double(char, int), less<char>
	si_type2_1.connect('a', [](char, int){return 8.3;}, mcurses::position::at_back);
	si_type2_1.connect('a', [](char, int){return 1.3;}, mcurses::position::at_back);
	si_type2_1.connect([](char, int){return 2.8;}, mcurses::position::at_front);
	si_type2_1.connect([](char, int){return 9.1;}, mcurses::position::at_front);
	si_type2_1.connect('z', [](char, int){return 7.3;}, mcurses::position::at_front);

	EXPECT_DOUBLE_EQ(7.3, *si_type2_1('f', 5));
	si_type2_1.disconnect('z');

	EXPECT_DOUBLE_EQ(1.3, *si_type2_1('f', 5));
	si_type2_1.disconnect('a');

	EXPECT_DOUBLE_EQ(9.1, *si_type2_1('k', 8));
}

// TEST_F(SignalImplTest, DisconnectBySlot)
// {
// 	// double(char, int), less<char>
// 	// si_type2_1.connect('a', [](char, int){return 8.3;}, mcurses::position::at_back);
// 	// si_type2_1.connect('a', [](char, int){return 1.3;}, mcurses::position::at_back);
// 	// si_type2_1.connect([](char, int){return 2.8;}, mcurses::position::at_front);
// 	// si_type2_1.connect([](char, int){return 9.1;}, mcurses::position::at_front);
// 	functor func = functor();
// 	si_type2_1.connect('z', func, mcurses::position::at_front);

// 	// EXPECT_DOUBLE_EQ(7.3, *si_type2_1('f', 5));
// 	si_type2_1.disconnect(func);

// 	EXPECT_DOUBLE_EQ(1.3, *si_type2_1('f', 5));
// 	// si_type2_1.disconnect('a');

// 	// EXPECT_DOUBLE_EQ(9.1, *si_type2_1('k', 8));
// }

TEST_F(SignalImplTest, DisconnectAllSlots)
{
	EXPECT_EQ(6, si_type3_2.num_slots());

	si_type3_2.disconnect_all_slots();

	EXPECT_EQ(0, si_type3_2.num_slots());
	EXPECT_TRUE(si_type3_2.empty());
}

TEST_F(SignalImplTest, Empty)
{
	EXPECT_TRUE(si_type1_1.empty());

	si_type1_1.disconnect_all_slots();

	EXPECT_TRUE(si_type1_1.empty());

	si_type3_2.disconnect_all_slots();
	EXPECT_TRUE(si_type3_2.empty());
}

TEST_F(SignalImplTest, NumSlots)
{
	EXPECT_EQ(0, si_type1_1.num_slots());
	EXPECT_EQ(1, si_type1_2.num_slots());
	EXPECT_EQ(3, si_type2_2.num_slots());
	EXPECT_EQ(6, si_type3_2.num_slots());
}

TEST_F(SignalImplTest, OperatorParenthesisCall)
{
	// call on empty signal that returns void
	si_type1_1(1);

	// call on empty signal that does not return void
	EXPECT_FALSE(bool(si_type2_1('j', 5)));

	// call on void return type signal that contains slots
	si_type1_2(8);

	// call on non-void return type signal and check return value
	ASSERT_TRUE(bool(si_type2_2('r', 8)));
	EXPECT_DOUBLE_EQ(9.1, *si_type2_2('r', 8));

	// signal_impl with extended slots
	si_type4.connect_extended(boost_extended_slot, mcurses::position::at_back);
	ASSERT_TRUE(bool(si_type4(6, 3.2)));
	EXPECT_EQ('h', *si_type4(6, 3.2));
}

TEST_F(SignalImplTest, ConstOperatorParenthesisCall)
{
	// call on empty signal that returns void
	const type_1 si_1 = si_type1_1;
	si_1(3);

	// call on empty signal that does not return void
	const type_2 si_2 = si_type2_1;
	EXPECT_FALSE(bool(si_2('j', 5)));

	// call on void return type signal that contains slots
	const type_1 si_3 = si_type1_2;
	si_3(8);

	// call on non-void return type signal and check return value
	const type_2 si_4 = si_type2_2;
	ASSERT_TRUE(bool(si_4('r', 8)));
	EXPECT_DOUBLE_EQ(9.1, *si_4('r', 8));

	// signal_impl with extended slots
	si_type4.connect_extended(boost_extended_slot, mcurses::position::at_back);
	const type_4 si_5 = si_type4;
	ASSERT_TRUE(bool(si_5(6, 3.2)));
	EXPECT_EQ('h', *si_5(6, 3.2));
}

TEST_F(SignalImplTest, Combiner)
{
	EXPECT_TRUE(typeid(si_type2_2.combiner()) == typeid(mcurses::optional_last_value<double>{}));

	auto comb = si_type3_1.combiner();
	std::vector<int> vec {1,2,3,4,5};
	auto i = comb(std::begin(vec), std::end(vec));
	ASSERT_TRUE(bool(i));
	EXPECT_EQ(5, *i);
}

TEST_F(SignalImplTest, SetCombiner)
{
	si_type1_2.set_combiner(mcurses::optional_last_value<void>{});
	EXPECT_TRUE(typeid(si_type1_2.combiner()) == typeid(mcurses::optional_last_value<void>{}));

	si_type2_2.set_combiner(mcurses::optional_last_value<double>{});
	EXPECT_TRUE(typeid(si_type2_2.combiner()) == typeid(mcurses::optional_last_value<double>{}));
}