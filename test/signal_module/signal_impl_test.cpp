#include <mcurses/signal_module/detail/signal_impl.hpp>
#include <gtest/gtest.h>

#include <mcurses/signal_module/signal.hpp>
#include <mcurses/signal_module/optional_last_value.hpp>
#include <mcurses/signal_module/connection.hpp>
#include <mcurses/signal_module/position.hpp>
#include <functional>
#include <boost/function.hpp>
#include <type_traits>

// make a fixture here with appropriate signal impls, and slots, 'extended' slots etc..
// anything that signal_impl objects might need to test their functions.
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
				double, std::less<double>, boost::function<unsigned(long long)>> type_3;

	SignalImplTest()
	{
		signal_std.connect(boost_slot_empty1);
		signal_std.connect(std_slot_empty1);
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

	// type_3 - unsigned(long long), less<double>, boost::function
	type_3 si_type3_1{mcurses::optional_last_value<unsigned>(), std::less<double>()};
	type_3 si_type3_2{mcurses::optional_last_value<unsigned>(), std::less<double>()};

	// Slots - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Empty Slot - boost::function
	mcurses::slot<void(int), boost::function<void(int)>> boost_slot_empty1{};

	// Empty Slot - boost::function
	mcurses::slot<unsigned(long long), boost::function<unsigned(long long)>> boost_slot_empty2{};

	// Function Slot - boost::function
	mcurses::slot<void(unsigned), boost::function<void(unsigned)>> boost_slot_function1{[](unsigned){return;}};

	// Function Slot - boost::function
	mcurses::slot<double(char), boost::function<double(char)>> boost_slot_function2{[](char){return 3.4;}};

	// Extended Slot - boost::function
	mcurses::slot<char(const mcurses::connection&, int, double), boost::function<char(const mcurses::connection&, int, double)>>
			boost_extended_slot{[](const mcurses::connection&, int, double){return 'h';}};

	// // Slot holds empty Signal - boost::function
	mcurses::signal<unsigned(long long), mcurses::optional_last_value<unsigned>,
					int, std::less<int>, boost::function<unsigned(long long)>> empty_signal_boost{};
	mcurses::slot<mcurses::optional<unsigned>(long long), boost::function<mcurses::optional<unsigned>(long long)>>
					boost_slot_holds_empty_signal{empty_signal_boost};

	// // Slot holds non-empty Signal - boost::function
	mcurses::signal<unsigned(long long), mcurses::optional_last_value<unsigned>,
					int, std::less<int>, boost::function<unsigned(long long)>> signal_boost{};
	mcurses::slot<mcurses::optional<unsigned>(long long), boost::function<mcurses::optional<unsigned>(long long)>>
					boost_slot_holds_signal{};

	// Empty Slot - std::function
	mcurses::slot<void(int)> std_slot_empty1{};

	// Empty Slot - std::function
	mcurses::slot<unsigned(long long)> std_slot_empty2{};

	// Function Slot - std::function
	mcurses::slot<void(unsigned)> std_slot_function1{[](unsigned){return;}};

	// Function Slot - std::function
	mcurses::slot<double(char)> std_slot_function2{[](char){return 3.4;}};

	// Extended Slot - std::function
	mcurses::slot<char(const mcurses::connection&, int, double)> std_extended_slot{[](const mcurses::connection&, int, double){return 'h';}};

	// Slot holds empty Signal - std::function
	mcurses::signal<void(int)> empty_signal_std{};
	mcurses::slot<void(int)> std_slot_holds_empty_signal{empty_signal_std};

	// Slot holds non-empty Signal - std::function
	mcurses::signal<void(int)> signal_std{};
	mcurses::slot<void(int)> std_slot_holds_signal{};
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