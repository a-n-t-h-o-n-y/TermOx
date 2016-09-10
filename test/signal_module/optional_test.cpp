#include <mcurses/signal_module/optional.hpp>
#include <mcurses/signal_module/none.hpp>
#include <mcurses/signal_module/bad_optional_access.hpp>
#include <gtest/gtest.h>
#include <string>

TEST(OptionalTest, DefaultConstructor)
{
	mcurses::optional<int> opt{};
	EXPECT_FALSE(bool(opt));
}

TEST(OptionalTest, NoneTypeConstructor)
{
	mcurses::optional<int> opt{mcurses::none};
	EXPECT_FALSE(bool(opt));
}

TEST(OptionalTest, ConstLValueConstructor)
{
	int i = 5;
	mcurses::optional<int> opt1{i};
	ASSERT_TRUE(bool(opt1));
	EXPECT_EQ(5, *opt1);

	const int ii = -7;
	mcurses::optional<int> opt2{ii};
	ASSERT_TRUE(bool(opt2));
	EXPECT_EQ(-7, *opt2);

	const char c = 'k';
	mcurses::optional<int> opt3{c};
	ASSERT_TRUE(bool(opt3));
	EXPECT_EQ('k', *opt3);

	const char& c_r = c;
	mcurses::optional<int> opt4{c_r};
	ASSERT_TRUE(bool(opt4));
	EXPECT_EQ('k', *opt4);
}

TEST(OptionalTest, RValueConstructor)
{
	mcurses::optional<int> opt1{7};
	ASSERT_TRUE(bool(opt1));
	EXPECT_EQ(7, *opt1);

	mcurses::optional<char> opt2{'k'};
	ASSERT_TRUE(bool(opt2));
	EXPECT_EQ('k', *opt2);

	int i = -8;
	mcurses::optional<int> opt3{std::move(i)};
	ASSERT_TRUE(bool(opt3));
	EXPECT_EQ(-8, *opt3);
}

TEST(OptionalTest, ConditionalConstructor)
{
	const char c = 'k';
	const char& c_r = c;
	bool cond_t = true;
	bool cond_f = false;

	mcurses::optional<int> opt1{true, 7};
	ASSERT_TRUE(bool(opt1));
	EXPECT_EQ(7, *opt1);

	mcurses::optional<int> opt2{false, 7};
	EXPECT_FALSE(bool(opt2));

	mcurses::optional<char> opt3{cond_t, c};
	ASSERT_TRUE(bool(opt3));
	EXPECT_EQ('k', *opt3);

	mcurses::optional<char> opt4{cond_f, c_r};
	EXPECT_FALSE(bool(opt4));
	
	mcurses::optional<char> opt5{cond_t, c_r};
	EXPECT_TRUE(bool(opt5));
	EXPECT_EQ('k', *opt5);
}

TEST(OptionalTest, CopyConstructor)
{
	mcurses::optional<int> opt1{7};
	mcurses::optional<int> opt2{opt1};
	ASSERT_TRUE(bool(opt2));
	EXPECT_EQ(7, *opt2);

	mcurses::optional<double> opt3{};
	mcurses::optional<double> opt4{opt3};
	EXPECT_FALSE(bool(opt4));
	EXPECT_EQ(nullptr, opt4.get_ptr());
}

TEST(OptionalTest, MoveConstructor)
{
	mcurses::optional<int> opt1{9};
	int* ip = opt1.get_ptr();
	mcurses::optional<int> opt2{std::move(opt1)};
	ASSERT_TRUE(bool(opt2));
	EXPECT_EQ(9, *opt2);
	EXPECT_EQ(ip, opt2.get_ptr());

	EXPECT_FALSE(bool(opt1));
	EXPECT_EQ(nullptr, opt1.get_ptr());

	mcurses::optional<int> opt3{mcurses::optional<int>{-5}};
	ASSERT_TRUE(bool(opt3));
	EXPECT_EQ(-5, *opt3);

	mcurses::optional<int> opt4{mcurses::optional<int>{mcurses::none}};
	EXPECT_FALSE(bool(opt4));

	mcurses::optional<double> opt5{};
	mcurses::optional<double> opt6{std::move(opt5)};
	EXPECT_FALSE(bool(opt6));
	EXPECT_EQ(nullptr, opt6.get_ptr());

	mcurses::optional<double> opt7{mcurses::optional<double>{}};
	EXPECT_FALSE(bool(opt7));
	EXPECT_EQ(nullptr, opt7.get_ptr());
}

TEST(OptionalTest, FromConvertibleTypeCopyConstructor)
{
	mcurses::optional<int> opt_int{7};
	mcurses::optional<double> opt_dbl{opt_int};

	ASSERT_TRUE(bool(opt_dbl));
	EXPECT_DOUBLE_EQ(7, *opt_dbl);

	mcurses::optional<const char*> opt_c_str{"Hello"};
	mcurses::optional<std::string> opt_string{opt_c_str};

	ASSERT_TRUE(bool(opt_string));
	EXPECT_EQ("Hello", *opt_string);

	mcurses::optional<double> opt_dbl2{};
	mcurses::optional<int> opt_int2{opt_dbl2};
	EXPECT_FALSE(bool(opt_int2));
	EXPECT_EQ(nullptr, opt_int2.get_ptr());
}

TEST(OptionalTest, FromConvertibleTypeMoveConstructor)
{
	mcurses::optional<int> opt_int{2};
	mcurses::optional<double> opt_dbl{std::move(opt_int)};
	ASSERT_TRUE(bool(opt_dbl));
	EXPECT_DOUBLE_EQ(2, *opt_dbl);

	EXPECT_FALSE(bool(opt_int));
	EXPECT_EQ(nullptr, opt_int.get_ptr());

	mcurses::optional<double> opt_dbl2{};
	mcurses::optional<int> opt_int2{opt_dbl2};
	EXPECT_FALSE(bool(opt_int2));
	EXPECT_EQ(nullptr, opt_int2.get_ptr());
	EXPECT_EQ(nullptr, opt_dbl2.get_ptr());

	mcurses::optional<const char*> opt_c_str{"Hello"};
	mcurses::optional<std::string> opt_string{std::move(opt_c_str)};

	ASSERT_TRUE(bool(opt_string));
	EXPECT_EQ("Hello", *opt_string);
	EXPECT_FALSE(bool(opt_c_str));
	EXPECT_EQ(nullptr, opt_c_str.get_ptr());
}

TEST(OptionalTest, NoneAssignementOperator)
{
	mcurses::optional<int> opt{8};
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(8, *opt);

	opt = mcurses::none;
	EXPECT_FALSE(bool(opt));
	EXPECT_EQ(nullptr, opt.get_ptr());
}

TEST(OptionalTest, LValueTAssignmentOperator)
{
	mcurses::optional<int> opt{-77};
	int i = 45;
	opt = i;
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(45, *opt);

	const int c_i = 99;
	opt = c_i;
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(99, *opt);

}

TEST(OptionalTest, RValueTAssignmentOperator)
{
	mcurses::optional<int> opt1{};
	opt1 = 99;
	ASSERT_TRUE(bool(opt1));
	EXPECT_EQ(99, *opt1);

	mcurses::optional<int> opt2{-77};
	opt2 = 88;
	ASSERT_TRUE(bool(opt2));
	EXPECT_EQ(88, *opt2);
}


TEST(OptionalTest, CopyAssignmentOperator)
{
	mcurses::optional<int> opt1{99};
	mcurses::optional<int> opt2{7};

	opt1 = opt2;

	ASSERT_TRUE(bool(opt1));
	EXPECT_EQ(7, *opt1);

	mcurses::optional<int> opt3{0};
	EXPECT_EQ(0, *(opt2 = opt3));

	mcurses::optional<int> opt_empty{};
	opt1 = opt_empty;

	EXPECT_FALSE(bool(opt1));
	EXPECT_EQ(nullptr, opt1.get_ptr());
}

TEST(OptionalTest, MoveAssignmentOperator)
{
	mcurses::optional<int> opt1{5};
	mcurses::optional<int> opt2{};

	opt2 = std::move(opt1);

	EXPECT_FALSE(bool(opt1));
	EXPECT_EQ(nullptr, opt1.get_ptr());
	ASSERT_TRUE(bool(opt2));
	EXPECT_EQ(5, *opt2);

	mcurses::optional<int> opt3{};
	opt3 = mcurses::optional<int>{9};

	ASSERT_TRUE(bool(opt3));
	EXPECT_EQ(9, *opt3);
}

TEST(OptionalTest, ConversionCopyAssignmentConstructor)
{
	mcurses::optional<int> opt_int{7};
	mcurses::optional<double> opt_dbl{10.0};

	opt_dbl = opt_int;

	ASSERT_TRUE(bool(opt_dbl));
	EXPECT_DOUBLE_EQ(7, *opt_dbl);
	ASSERT_TRUE(bool(opt_int));
	EXPECT_EQ(7, *opt_int);

	mcurses::optional<const char*> opt_c_str{"Hello"};
	mcurses::optional<std::string> opt_string{"World!"};

	opt_string = opt_c_str;

	ASSERT_TRUE(bool(opt_string));
	EXPECT_EQ("Hello", *opt_string);
	ASSERT_TRUE(bool(opt_c_str));
	EXPECT_EQ("Hello", *opt_c_str);

	mcurses::optional<long int> opt_long{4321};
	EXPECT_EQ(7, *(opt_long = opt_int));

	mcurses::optional<int> opt_empty{};
	mcurses::optional<double> opt_dbl2{8};

	opt_dbl2 = opt_empty;

	EXPECT_FALSE(bool(opt_dbl2));
	EXPECT_EQ(nullptr, opt_dbl2.get_ptr());
}

TEST(OptionalTest, ConversionMoveAssignmentOperator)
{
	mcurses::optional<int> opt_int{6};
	mcurses::optional<double> opt_dbl{8.4};

	opt_dbl = std::move(opt_int);

	ASSERT_TRUE(bool(opt_dbl));
	EXPECT_DOUBLE_EQ(6, *opt_dbl);
	EXPECT_FALSE(bool(opt_int));
	EXPECT_EQ(nullptr, opt_int.get_ptr());

	opt_dbl = mcurses::optional<int>{99};

	ASSERT_TRUE(bool(opt_dbl));
	EXPECT_DOUBLE_EQ(99, *opt_dbl);

	opt_dbl = mcurses::optional<int>{};

	EXPECT_FALSE(bool(opt_dbl));
	EXPECT_EQ(nullptr, opt_dbl.get_ptr());
}

TEST(OptionalTest, Emplace)
{
	mcurses::optional<int> opt1{};
	opt1.emplace(6);

	ASSERT_TRUE(bool(opt1));
	EXPECT_EQ(6, *opt1);

	mcurses::optional<std::string> opt2{};
	opt2.emplace(3, 'h');

	ASSERT_TRUE(bool(opt2));
	EXPECT_EQ("hhh", *opt2);

	opt2.emplace(std::string{"Hello!"});

	ASSERT_TRUE(bool(opt2));
	EXPECT_EQ("Hello!", *opt2);

	opt2.emplace();
	ASSERT_TRUE(bool(opt2));
	EXPECT_EQ("", *opt2);
}

TEST(OptionalTest, ConstGet)
{
	const mcurses::optional<int> opt{9};
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(9, opt.get());
}

TEST(OptionalTest, Get)
{
	mcurses::optional<int> opt{7};
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(7, opt.get());

	opt.get() = 9;
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(9, opt.get());
}

TEST(OptionalTest, ConstArrowOperator)
{
	const mcurses::optional<std::string> opt{"Hello, World!"};
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(13, opt->size());
}

TEST(OptionalTest, ArrowOperator)
{
	mcurses::optional<std::string> opt{"Hello"};
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(5, opt->size());
	opt->append(", World!");
	EXPECT_EQ("Hello, World!", *opt);
}

TEST(OptionalTest, ConstLValueDereferenceOperator)
{
	const mcurses::optional<int> opt_int{88};
	ASSERT_TRUE(bool(opt_int));
	EXPECT_EQ(88, *opt_int);

	const mcurses::optional<std::string> opt_string{"Hello!"};
	ASSERT_TRUE(bool(opt_string));
	EXPECT_EQ(6, (*opt_string).size());
}

TEST(OptionalTest, LValueDereferenceOperator)
{
	mcurses::optional<std::string> opt_string{"Hello"};
	ASSERT_TRUE(bool(opt_string));
	EXPECT_EQ(5, (*opt_string).size());

	(*opt_string).append(", World!");
	ASSERT_TRUE(bool(opt_string));
	EXPECT_EQ("Hello, World!", *opt_string);
}

TEST(OptionalTest, RValueDereferenceOperator)
{
	EXPECT_EQ(6, (*mcurses::optional<std::string>{"Hello!"}).size());
	EXPECT_EQ(77 , *mcurses::optional<int>{77});

	mcurses::optional<std::string> opt{"Hello, World!"};
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ("Hello, World!", *std::move(opt));
	// EXPECT_EQ(0, opt->size()); // seg-fault
}

TEST(OptionalTest, ConstLValueValueMethod)
{
	const mcurses::optional<int> opt1{};
	EXPECT_THROW(opt1.value(), mcurses::bad_optional_access);

	const mcurses::optional<int> opt2{8};
	ASSERT_TRUE(bool(opt2));
	EXPECT_NO_THROW(opt2.value());
	EXPECT_EQ(8, opt2.value());
}

TEST(OptionalTest, LValueValueMethod)
{
	mcurses::optional<int> opt1{};
	EXPECT_THROW(opt1.value(), mcurses::bad_optional_access);

	mcurses::optional<int> opt2{8};
	ASSERT_TRUE(bool(opt2));
	EXPECT_NO_THROW(opt2.value());
	EXPECT_EQ(8, opt2.value());

	mcurses::optional<std::string> opt_string{"Hello"};
	opt_string.value().append(", World!");
	ASSERT_TRUE(bool(opt_string));
	EXPECT_EQ("Hello, World!", *opt_string);
}

TEST(OptionalTest, RValueValueMethod)
{
	mcurses::optional<int> opt1{};
	EXPECT_THROW(std::move(opt1).value(), mcurses::bad_optional_access);
	EXPECT_FALSE(bool(opt1));
	EXPECT_EQ(nullptr, opt1.get_ptr());

	mcurses::optional<int> opt2{8};
	ASSERT_TRUE(bool(opt2));
	EXPECT_NO_THROW(std::move(opt2).value());
	EXPECT_FALSE(bool(opt2));
	EXPECT_EQ(nullptr, opt2.get_ptr());

	mcurses::optional<int> opt3{10};
	ASSERT_TRUE(bool(opt3));
	EXPECT_EQ(10, std::move(opt3).value());
	EXPECT_FALSE(bool(opt3));
	EXPECT_EQ(nullptr, opt3.get_ptr());

	EXPECT_EQ(6, (mcurses::optional<int>{6}).value());
	EXPECT_NO_THROW((mcurses::optional<int>{6}).value());
	EXPECT_THROW((mcurses::optional<int>{}).value(), mcurses::bad_optional_access);
}

TEST(OptionalTest, ConstLValueValueOr)
{
	mcurses::optional<int> opt{8};
	ASSERT_TRUE(bool(opt));
	EXPECT_DOUBLE_EQ(8, opt.value_or(7.2));

	mcurses::optional<int> opt_empty{};
	EXPECT_FALSE(bool(opt_empty));
	int i = 32;
	EXPECT_EQ(i, opt_empty.value_or(i));
	double d = 6.3;
	EXPECT_EQ(6, opt_empty.value_or(d));
}

TEST(OptionalTest, RValueValueOr)
{
	mcurses::optional<int> opt1{};
	double d = 9.3;
	EXPECT_EQ(9, std::move(opt1).value_or(d));
	EXPECT_FALSE(bool(opt1));
	EXPECT_EQ(nullptr, opt1.get_ptr());

	mcurses::optional<int> opt2{8};
	ASSERT_TRUE(bool(opt2));
	EXPECT_EQ(8, std::move(opt2).value_or(9.3));
	EXPECT_FALSE(bool(opt2));
	EXPECT_EQ(nullptr, opt2.get_ptr());

	mcurses::optional<int> opt3{10};
	ASSERT_TRUE(bool(opt3));
	EXPECT_EQ(10, std::move(opt3).value_or(843));
	EXPECT_FALSE(bool(opt3));
	EXPECT_EQ(nullptr, opt3.get_ptr());

	EXPECT_EQ(6, (mcurses::optional<int>{6}).value_or(99));
	EXPECT_EQ(99, (mcurses::optional<int>{}).value_or(99));
}

double f() {return 7.7;}

TEST(OptionalTest, ConstLValueValueOrEval)
{
	mcurses::optional<int> opt{5};
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(5, opt.value_or_eval([](){return 8;}));

	opt = mcurses::none;
	EXPECT_FALSE(bool(opt));
	EXPECT_EQ(8, opt.value_or_eval([](){return 8;}));

	EXPECT_EQ(7, opt.value_or_eval(f));
	opt = -4;
	EXPECT_EQ(-4, opt.value_or_eval(f));
}

TEST(OptionalTest, RValueValueOrEval)
{
	EXPECT_EQ(3, (mcurses::optional<int>{3}).value_or_eval([](){return 8;}));
	
	mcurses::optional<int> opt1{};
	EXPECT_EQ(2, opt1.value_or_eval([](){return 2;}));
	EXPECT_FALSE(bool(opt1));
	EXPECT_EQ(nullptr, opt1.get_ptr());

	mcurses::optional<int> opt2{88};
	EXPECT_EQ(88, std::move(opt2).value_or_eval([](){return 3;}));
	EXPECT_FALSE(bool(opt2));
	EXPECT_EQ(nullptr, opt2.get_ptr());
}

TEST(OptionalTest, ConstGetPtr)
{
	const mcurses::optional<std::string> opt{"Hello!"};
	EXPECT_EQ("Hello!", *(opt.get_ptr()));
}

TEST(OptionalTest, GetPtr)
{
	mcurses::optional<std::string> opt{"Hello"};
	EXPECT_EQ("Hello", *(opt.get_ptr()));
	opt.get_ptr()->append(", World!");
	EXPECT_EQ("Hello, World!", *(opt.get_ptr()));
}

TEST(OptionalTest, OperatorBool)
{
	const mcurses::optional<int> opt1{8};
	EXPECT_TRUE(bool(opt1));

	const mcurses::optional<int> opt2{};
	EXPECT_FALSE(bool(opt2));

	mcurses::optional<int> opt3{8};
	EXPECT_TRUE(bool(opt3));

	mcurses::optional<int> opt4{};
	EXPECT_FALSE(bool(opt4));

	EXPECT_TRUE(bool(mcurses::optional<int>{4}));
	EXPECT_FALSE(bool(mcurses::optional<int>{}));
	const mcurses::optional<int> opt5{4};
	EXPECT_TRUE(bool(std::move(opt5)));
	const mcurses::optional<int> opt6{};
	EXPECT_FALSE(bool(std::move(opt6)));
}

TEST(OptionalTest, OperatorNotEqual)
{
	const mcurses::optional<int> opt1{8};
	EXPECT_FALSE(!opt1);

	const mcurses::optional<int> opt2{};
	EXPECT_TRUE(!opt2);

	mcurses::optional<int> opt3{8};
	EXPECT_FALSE(!opt3);

	mcurses::optional<int> opt4{};
	EXPECT_TRUE(!opt4);

	EXPECT_FALSE(!mcurses::optional<int>{4});
	EXPECT_TRUE(!mcurses::optional<int>{});
	const mcurses::optional<int> opt5{4};
	EXPECT_FALSE(!std::move(opt5));
	const mcurses::optional<int> opt6{};
	EXPECT_TRUE(!std::move(opt6));
}

TEST(OptionalTest, OperatorEqualTo)
{
	mcurses::optional<int> opt1{5};
	mcurses::optional<int> opt2{5};

	EXPECT_TRUE(opt1 == opt2);

	mcurses::optional<int> opt3{};
	mcurses::optional<int> opt4{};

	EXPECT_TRUE(opt3 == opt4);

	mcurses::optional<int> opt5{};
	mcurses::optional<int> opt6{8};

	EXPECT_FALSE(opt5 == opt6);
	EXPECT_FALSE(opt6 == opt5);

	EXPECT_FALSE(opt6 == opt1);

	EXPECT_TRUE(opt1 == opt1);
	EXPECT_TRUE(opt3 == opt3);
}

TEST(OptionalTest, OperatorNotEqualTo)
{
	mcurses::optional<int> opt1{5};
	mcurses::optional<int> opt2{5};

	EXPECT_FALSE(opt1 != opt2);

	mcurses::optional<int> opt3{};
	mcurses::optional<int> opt4{};

	EXPECT_FALSE(opt3 != opt4);

	mcurses::optional<int> opt5{};
	mcurses::optional<int> opt6{8};

	EXPECT_TRUE(opt5 != opt6);
	EXPECT_TRUE(opt6 != opt5);

	EXPECT_TRUE(opt6 != opt1);

	EXPECT_FALSE(opt1 != opt1);
	EXPECT_FALSE(opt3 != opt3);
}

TEST(OptionalTest, OperatorLessThan)
{
	mcurses::optional<int> opt1{1};
	mcurses::optional<int> opt2{2};
	mcurses::optional<int> opt3{};
	mcurses::optional<int> opt4{};

	EXPECT_TRUE(opt1 < opt2);
	EXPECT_FALSE(opt2 < opt1);

	EXPECT_FALSE(opt1 < opt1);
	EXPECT_FALSE(opt3 < opt3);

	EXPECT_FALSE(opt3 < opt4);
	EXPECT_TRUE(opt3 < opt2);
	EXPECT_FALSE(opt1 < opt3);
}

TEST(OptionalTest, OperatorGreaterThan)
{
	mcurses::optional<int> opt1{1};
	mcurses::optional<int> opt2{2};
	mcurses::optional<int> opt3{};
	mcurses::optional<int> opt4{};

	EXPECT_FALSE(opt1 > opt2);
	EXPECT_TRUE(opt2 > opt1);

	EXPECT_FALSE(opt1 > opt1);
	EXPECT_FALSE(opt3 > opt3);

	EXPECT_FALSE(opt3 > opt4);
	EXPECT_FALSE(opt3 > opt2);
	EXPECT_TRUE(opt1 > opt3);
}

TEST(OptionalTest, OperatorLessThanOrEqualTo)
{
	mcurses::optional<int> opt1{1};
	mcurses::optional<int> opt2{2};
	mcurses::optional<int> opt3{};
	mcurses::optional<int> opt4{};
	mcurses::optional<int> opt5{2};

	EXPECT_TRUE(opt1 <= opt2);
	EXPECT_FALSE(opt2 <= opt1);
	EXPECT_TRUE(opt5 <= opt2);
	EXPECT_TRUE(opt2 <= opt5);

	EXPECT_TRUE(opt2 <= opt2);
	EXPECT_TRUE(opt3 <= opt3);

	EXPECT_TRUE(opt3 <= opt4);
	EXPECT_TRUE(opt3 <= opt2);
	EXPECT_FALSE(opt1 <= opt3);
}

TEST(OptionalTest, OperatorGreaterThanOrEqualTo)
{
	mcurses::optional<int> opt1{1};
	mcurses::optional<int> opt2{2};
	mcurses::optional<int> opt3{};
	mcurses::optional<int> opt4{};
	mcurses::optional<int> opt5{2};

	EXPECT_FALSE(opt1 >= opt2);
	EXPECT_TRUE(opt2 >= opt1);
	EXPECT_TRUE(opt5 >= opt2);
	EXPECT_TRUE(opt2 >= opt5);

	EXPECT_TRUE(opt2 >= opt2);
	EXPECT_TRUE(opt3 >= opt3);

	EXPECT_TRUE(opt3 >= opt4);
	EXPECT_FALSE(opt3 >= opt2);
	EXPECT_TRUE(opt1 >= opt3);
}

TEST(OptionalTest, OperatorEqualToNoneT)
{
	mcurses::optional<int> opt1{7};
	mcurses::optional<int> opt2{};

	EXPECT_FALSE(opt1 == mcurses::none);
	EXPECT_TRUE(opt2 == mcurses::none);
	EXPECT_FALSE(mcurses::none == opt1);
	EXPECT_TRUE(mcurses::none == opt2);

	opt2 = std::move(opt1);

	EXPECT_FALSE(opt2 == mcurses::none);
	EXPECT_TRUE(opt1 == mcurses::none);
	EXPECT_FALSE(mcurses::none == opt2);
	EXPECT_TRUE(mcurses::none == opt1);
}

TEST(OptionalTest, OperatorNotEqualToNoneT)
{
	mcurses::optional<int> opt1{7};
	mcurses::optional<int> opt2{};

	EXPECT_TRUE(opt1 != mcurses::none);
	EXPECT_FALSE(opt2 != mcurses::none);
	EXPECT_TRUE(mcurses::none != opt1);
	EXPECT_FALSE(mcurses::none != opt2);

	opt2 = std::move(opt1);
	
	EXPECT_TRUE(opt2 != mcurses::none);
	EXPECT_FALSE(opt1 != mcurses::none);
	EXPECT_TRUE(mcurses::none != opt2);
	EXPECT_FALSE(mcurses::none != opt1);
}

TEST(OptionalTest, MakeOptional)
{
	auto opt1 = mcurses::make_optional(7);
	ASSERT_TRUE(bool(opt1));
	EXPECT_EQ(7, *opt1);
	EXPECT_TRUE(opt1 == mcurses::make_optional(7));

	auto opt2 = mcurses::make_optional('k');
	ASSERT_TRUE(bool(opt2));
	EXPECT_EQ('k', *opt2);
	EXPECT_TRUE(opt2 == mcurses::make_optional('k'));
}

TEST(OptionalTest, ConditionalMakeOptional)
{
	auto opt1 = mcurses::make_optional(true, 7);
	ASSERT_TRUE(bool(opt1));
	EXPECT_EQ(7, *opt1);
	EXPECT_TRUE(opt1 == mcurses::make_optional(7));

	auto opt2 = mcurses::make_optional(false, 'k');
	EXPECT_FALSE(bool(opt2));
	EXPECT_EQ(nullptr, opt2.get_ptr());
	EXPECT_TRUE(opt2 == mcurses::none);
}

TEST(OptionalTest, GetConstFreeFunction)
{
	int i = 7;
	const mcurses::optional<int> opt(i);

	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(7, get(opt));
}

TEST(OptionalTest, GetFreeFunction)
{
	int i = 7;
	mcurses::optional<int> opt(i);

	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(7, get(opt));

	get(opt) = 3;
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(3, get(opt));
}

TEST(OptionalTest, ConstGetFromPointerFreeFunction)
{
	const mcurses::optional<int> opt{4};
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(4, *(get(&opt)));
}

TEST(OptionalTest, GetFromPointerFreeFunction)
{
	mcurses::optional<int> opt{4};
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(4, *(get(&opt)));

	*(get(&opt)) = 5;
	EXPECT_EQ(5, opt.get());
}

TEST(OptionalTest, ConstGetPointerFreeFunction)
{
	const mcurses::optional<int> opt{8};
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(8, *get_pointer(opt));
}

TEST(OptionalTest, GetPointerFreeFunction)
{
	mcurses::optional<int> opt{8};
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(8, *get_pointer(opt));

	*get_pointer(opt) = 4;
	ASSERT_TRUE(bool(opt));
	EXPECT_EQ(4, opt.get());
}

TEST(OptionalTest, Swap)
{
	mcurses::optional<int> opt1{};
	mcurses::optional<int> opt2{};
	mcurses::optional<int> opt3{6};
	mcurses::optional<int> opt4{32};

	mcurses::swap(opt1, opt2);
	EXPECT_FALSE(bool(opt1));
	EXPECT_FALSE(bool(opt2));
	EXPECT_EQ(nullptr, opt1.get_ptr());
	EXPECT_EQ(nullptr, opt2.get_ptr());

	mcurses::swap(opt3, opt4);
	EXPECT_TRUE(bool(opt3));
	EXPECT_TRUE(bool(opt3));
	EXPECT_EQ(32, opt3.get());
	EXPECT_EQ(6, opt4.get());

	mcurses::swap(opt3, opt2);
	EXPECT_FALSE(bool(opt3));
	EXPECT_TRUE(bool(opt2));
	EXPECT_EQ(32, opt2.get());
	EXPECT_EQ(nullptr, opt3.get_ptr());

	mcurses::swap(opt1, opt4);
	EXPECT_FALSE(bool(opt4));
	EXPECT_TRUE(bool(opt1));
	EXPECT_EQ(6, opt1.get());
	EXPECT_EQ(nullptr, opt4.get_ptr());
}