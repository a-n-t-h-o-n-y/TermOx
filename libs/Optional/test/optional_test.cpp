#include "optional/bad_optional_access.hpp"
#include "optional/none.hpp"
#include "optional/optional.hpp"
#include <gtest/gtest.h>
#include <string>
#include <utility>

using opt::Optional;
using opt::none;
using opt::Bad_optional_access;

TEST(OptionalTest, DefaultConstructor) {
    Optional<int> opt{};
    Optional<void> ov;
    EXPECT_FALSE(bool(opt));
}

TEST(OptionalTest, NoneTypeConstructor) {
    Optional<int> opt{none};
    EXPECT_FALSE(bool(opt));
}

TEST(OptionalTest, ConstLValueConstructor) {
    int i = 5;
    Optional<int> opt1{i};
    ASSERT_TRUE(bool(opt1));
    EXPECT_EQ(5, *opt1);

    const int ii = -7;
    Optional<int> opt2{ii};
    ASSERT_TRUE(bool(opt2));
    EXPECT_EQ(-7, *opt2);

    const char c = 'k';
    Optional<int> opt3{c};
    ASSERT_TRUE(bool(opt3));
    EXPECT_EQ('k', *opt3);

    const char& c_r = c;
    Optional<int> opt4{c_r};
    ASSERT_TRUE(bool(opt4));
    EXPECT_EQ('k', *opt4);
}

TEST(OptionalTest, RValueConstructor) {
    Optional<int> opt1{7};
    ASSERT_TRUE(bool(opt1));
    EXPECT_EQ(7, *opt1);

    Optional<char> opt2{'k'};
    ASSERT_TRUE(bool(opt2));
    EXPECT_EQ('k', *opt2);

    int i = -8;
    Optional<int> opt3{std::move(i)};
    ASSERT_TRUE(bool(opt3));
    EXPECT_EQ(-8, *opt3);
}

TEST(OptionalTest, ConditionalConstructor) {
    const char c = 'k';
    const char& c_r = c;
    bool cond_t = true;
    bool cond_f = false;

    Optional<int> opt1{true, 7};
    ASSERT_TRUE(bool(opt1));
    EXPECT_EQ(7, *opt1);

    Optional<int> opt2{false, 7};
    EXPECT_FALSE(bool(opt2));

    Optional<char> opt3{cond_t, c};
    ASSERT_TRUE(bool(opt3));
    EXPECT_EQ('k', *opt3);

    Optional<char> opt4{cond_f, c_r};
    EXPECT_FALSE(bool(opt4));

    Optional<char> opt5{cond_t, c_r};
    EXPECT_TRUE(bool(opt5));
    EXPECT_EQ('k', *opt5);
}

TEST(OptionalTest, CopyConstructor) {
    Optional<int> opt1{7};
    Optional<int> opt2{opt1};
    ASSERT_TRUE(bool(opt2));
    EXPECT_EQ(7, *opt2);

    Optional<double> opt3{};
    Optional<double> opt4{opt3};
    EXPECT_FALSE(bool(opt4));
}

TEST(OptionalTest, MoveConstructor) {
    Optional<int> opt1{9};
    Optional<int> opt2{std::move(opt1)};
    ASSERT_TRUE(bool(opt2));
    EXPECT_EQ(9, *opt2);

    EXPECT_FALSE(bool(opt1));

    Optional<int> opt3{Optional<int>{-5}};
    ASSERT_TRUE(bool(opt3));
    EXPECT_EQ(-5, *opt3);

    Optional<int> opt4{Optional<int>{none}};
    EXPECT_FALSE(bool(opt4));

    Optional<double> opt5{};
    Optional<double> opt6{std::move(opt5)};
    EXPECT_FALSE(bool(opt6));

    Optional<double> opt7{Optional<double>{}};
    EXPECT_FALSE(bool(opt7));
}

TEST(OptionalTest, FromConvertibleTypeCopyConstructor) {
    Optional<int> opt_int{7};
    Optional<double> opt_dbl{opt_int};

    ASSERT_TRUE(bool(opt_dbl));
    EXPECT_DOUBLE_EQ(7, *opt_dbl);

    Optional<const char*> opt_c_str{"Hello"};
    Optional<std::string> opt_string{opt_c_str};

    ASSERT_TRUE(bool(opt_string));
    EXPECT_EQ("Hello", *opt_string);

    Optional<double> opt_dbl2{};
    Optional<int> opt_int2{opt_dbl2};
    EXPECT_FALSE(bool(opt_int2));
}

TEST(OptionalTest, FromConvertibleTypeMoveConstructor) {
    Optional<int> opt_int{2};
    Optional<double> opt_dbl{std::move(opt_int)};
    ASSERT_TRUE(bool(opt_dbl));
    EXPECT_DOUBLE_EQ(2, *opt_dbl);

    EXPECT_FALSE(bool(opt_int));

    Optional<double> opt_dbl2{};
    Optional<int> opt_int2{opt_dbl2};
    EXPECT_FALSE(bool(opt_int2));

    Optional<const char*> opt_c_str{"Hello"};
    Optional<std::string> opt_string{std::move(opt_c_str)};

    ASSERT_TRUE(bool(opt_string));
    EXPECT_EQ("Hello", *opt_string);
    EXPECT_FALSE(bool(opt_c_str));
}

TEST(OptionalTest, NoneAssignementOperator) {
    Optional<int> opt{8};
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(8, *opt);

    opt = none;
    EXPECT_FALSE(bool(opt));
}

TEST(OptionalTest, LValueTAssignmentOperator) {
    Optional<int> opt{-77};
    int i = 45;
    opt = i;
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(45, *opt);

    const int c_i = 99;
    opt = c_i;
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(99, *opt);
}

TEST(OptionalTest, RValueTAssignmentOperator) {
    Optional<int> opt1{};
    opt1 = 99;
    ASSERT_TRUE(bool(opt1));
    EXPECT_EQ(99, *opt1);

    Optional<int> opt2{-77};
    opt2 = 88;
    ASSERT_TRUE(bool(opt2));
    EXPECT_EQ(88, *opt2);
}

TEST(OptionalTest, CopyAssignmentOperator) {
    Optional<int> opt1{99};
    Optional<int> opt2{7};

    opt1 = opt2;

    ASSERT_TRUE(bool(opt1));
    EXPECT_EQ(7, *opt1);

    Optional<int> opt3{0};
    EXPECT_EQ(0, *(opt2 = opt3));

    Optional<int> opt_empty{};
    opt1 = opt_empty;

    EXPECT_FALSE(bool(opt1));
}

TEST(OptionalTest, MoveAssignmentOperator) {
    Optional<int> opt1{5};
    Optional<int> opt2{};

    opt2 = std::move(opt1);

    EXPECT_FALSE(bool(opt1));
    ASSERT_TRUE(bool(opt2));
    EXPECT_EQ(5, *opt2);

    Optional<int> opt3{};
    opt3 = Optional<int>{9};

    ASSERT_TRUE(bool(opt3));
    EXPECT_EQ(9, *opt3);
}

TEST(OptionalTest, ConversionCopyAssignmentConstructor) {
    Optional<int> opt_int{7};
    Optional<double> opt_dbl{10.0};

    opt_dbl = opt_int;

    ASSERT_TRUE(bool(opt_dbl));
    EXPECT_DOUBLE_EQ(7, *opt_dbl);
    ASSERT_TRUE(bool(opt_int));
    EXPECT_EQ(7, *opt_int);

    Optional<const char*> opt_c_str{"Hello"};
    Optional<std::string> opt_string{"World!"};

    opt_string = opt_c_str;

    ASSERT_TRUE(bool(opt_string));
    EXPECT_EQ("Hello", *opt_string);
    ASSERT_TRUE(bool(opt_c_str));
    EXPECT_EQ("Hello", *opt_c_str);

    Optional<long int> opt_long{4321};
    EXPECT_EQ(7, *(opt_long = opt_int));

    Optional<int> opt_empty{};
    Optional<double> opt_dbl2{8};

    opt_dbl2 = opt_empty;

    EXPECT_FALSE(bool(opt_dbl2));
}

TEST(OptionalTest, ConversionMoveAssignmentOperator) {
    Optional<int> opt_int{6};
    Optional<double> opt_dbl{8.4};

    opt_dbl = std::move(opt_int);

    ASSERT_TRUE(bool(opt_dbl));
    EXPECT_DOUBLE_EQ(6, *opt_dbl);
    EXPECT_FALSE(bool(opt_int));

    opt_dbl = Optional<int>{99};

    ASSERT_TRUE(bool(opt_dbl));
    EXPECT_DOUBLE_EQ(99, *opt_dbl);

    opt_dbl = Optional<int>{};

    EXPECT_FALSE(bool(opt_dbl));
}

TEST(OptionalTest, Emplace) {
    Optional<int> opt1{};
    opt1.emplace(6);

    ASSERT_TRUE(bool(opt1));
    EXPECT_EQ(6, *opt1);

    Optional<std::string> opt2{};
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

TEST(OptionalTest, ConstGet) {
    const Optional<int> opt{9};
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(9, opt.get());
}

TEST(OptionalTest, Get) {
    Optional<int> opt{7};
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(7, opt.get());

    opt.get() = 9;
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(9, opt.get());
}

TEST(OptionalTest, ConstArrowOperator) {
    const Optional<std::string> opt{"Hello, World!"};
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(13, opt->size());
}

TEST(OptionalTest, ArrowOperator) {
    Optional<std::string> opt{"Hello"};
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(5, opt->size());
    opt->append(", World!");
    EXPECT_EQ("Hello, World!", *opt);
}

TEST(OptionalTest, ConstLValueDereferenceOperator) {
    const Optional<const int> opt_int{88};
    ASSERT_TRUE(bool(opt_int));
    EXPECT_EQ(88, *opt_int);

    const Optional<std::string> opt_string{"Hello!"};
    ASSERT_TRUE(bool(opt_string));
    EXPECT_EQ(6, (*opt_string).size());
}

TEST(OptionalTest, LValueDereferenceOperator) {
    Optional<std::string> opt_string{"Hello"};
    ASSERT_TRUE(bool(opt_string));
    EXPECT_EQ(5, (*opt_string).size());

    (*opt_string).append(", World!");
    ASSERT_TRUE(bool(opt_string));
    EXPECT_EQ("Hello, World!", *opt_string);
}

TEST(OptionalTest, RValueDereferenceOperator) {
    EXPECT_EQ(6, (*Optional<std::string>{"Hello!"}).size());
    EXPECT_EQ(77, *Optional<int>{77});

    Optional<std::string> opt{"Hello, World!"};
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ("Hello, World!", *std::move(opt));
    // EXPECT_EQ(0, opt->size()); // seg-fault
}

TEST(OptionalTest, ConstLValueValueMethod) {
    const Optional<int> opt1{};
    EXPECT_THROW(opt1.value(), Bad_optional_access);

    const Optional<int> opt2{8};
    ASSERT_TRUE(bool(opt2));
    EXPECT_NO_THROW(opt2.value());
    EXPECT_EQ(8, opt2.value());
}

TEST(OptionalTest, LValueValueMethod) {
    Optional<int> opt1{};
    EXPECT_THROW(opt1.value(), Bad_optional_access);

    Optional<int> opt2{8};
    ASSERT_TRUE(bool(opt2));
    EXPECT_NO_THROW(opt2.value());
    EXPECT_EQ(8, opt2.value());

    Optional<std::string> opt_string{"Hello"};
    opt_string.value().append(", World!");
    ASSERT_TRUE(bool(opt_string));
    EXPECT_EQ("Hello, World!", *opt_string);
}

TEST(OptionalTest, RValueValueMethod) {
    Optional<int> opt1{};
    EXPECT_THROW(std::move(opt1).value(), Bad_optional_access);
    EXPECT_FALSE(bool(opt1));

    Optional<int> opt2{8};
    ASSERT_TRUE(bool(opt2));
    EXPECT_NO_THROW(std::move(opt2).value());

    Optional<int> opt3{10};
    ASSERT_TRUE(bool(opt3));
    EXPECT_EQ(10, std::move(opt3).value());

    EXPECT_EQ(6, (Optional<int>{6}).value());
    EXPECT_NO_THROW((Optional<int>{6}).value());
    EXPECT_THROW((Optional<int>{}).value(), Bad_optional_access);
}

TEST(OptionalTest, ConstLValueValueOr) {
    Optional<int> opt{8};
    ASSERT_TRUE(bool(opt));
    EXPECT_DOUBLE_EQ(8, opt.value_or(7.2));

    Optional<int> opt_empty{};
    EXPECT_FALSE(bool(opt_empty));
    int i = 32;
    EXPECT_EQ(i, opt_empty.value_or(i));
    double d = 6.3;
    EXPECT_EQ(6, opt_empty.value_or(d));
}

TEST(OptionalTest, RValueValueOr) {
    Optional<int> opt1{};
    double d = 9.3;
    EXPECT_EQ(9, std::move(opt1).value_or(d));
    EXPECT_FALSE(bool(opt1));

    Optional<int> opt2{8};
    ASSERT_TRUE(bool(opt2));
    EXPECT_EQ(8, std::move(opt2).value_or(9.3));

    Optional<int> opt3{10};
    ASSERT_TRUE(bool(opt3));
    EXPECT_EQ(10, std::move(opt3).value_or(843));

    EXPECT_EQ(6, (Optional<int>{6}).value_or(99));
    EXPECT_EQ(99, (Optional<int>{}).value_or(99));
}

double f() {
    return 7.7;
}

TEST(OptionalTest, ConstLValueValueOrEval) {
    Optional<int> opt{5};
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(5, opt.value_or_eval([]() { return 8; }));

    opt = none;
    EXPECT_FALSE(bool(opt));
    EXPECT_EQ(8, opt.value_or_eval([]() { return 8; }));

    EXPECT_EQ(7, opt.value_or_eval(f));
    opt = -4;
    EXPECT_EQ(-4, opt.value_or_eval(f));
}

TEST(OptionalTest, RValueValueOrEval) {
    EXPECT_EQ(3, (Optional<int>{3}).value_or_eval([]() { return 8; }));

    Optional<int> opt1{};
    EXPECT_EQ(2, opt1.value_or_eval([]() { return 2; }));
    EXPECT_FALSE(bool(opt1));

    Optional<int> opt2{88};
    EXPECT_EQ(88, std::move(opt2).value_or_eval([]() { return 3; }));
}

TEST(OptionalTest, ConstGetPtr) {
    const Optional<std::string> opt{"Hello!"};
    EXPECT_EQ("Hello!", *(opt.get_ptr()));
}

TEST(OptionalTest, GetPtr) {
    Optional<std::string> opt{"Hello"};
    EXPECT_EQ("Hello", *(opt.get_ptr()));
    opt.get_ptr()->append(", World!");
    EXPECT_EQ("Hello, World!", *(opt.get_ptr()));
}

TEST(OptionalTest, OperatorBool) {
    const Optional<int> opt1{8};
    EXPECT_TRUE(bool(opt1));

    const Optional<int> opt2{};
    EXPECT_FALSE(bool(opt2));

    Optional<int> opt3{8};
    EXPECT_TRUE(bool(opt3));

    Optional<int> opt4{};
    EXPECT_FALSE(bool(opt4));

    EXPECT_TRUE(bool(Optional<int>{4}));
    EXPECT_FALSE(bool(Optional<int>{}));
    const Optional<int> opt5{4};
    EXPECT_TRUE(bool(std::move(opt5)));
    const Optional<int> opt6{};
    EXPECT_FALSE(bool(std::move(opt6)));
}

TEST(OptionalTest, OperatorNotEqual) {
    const Optional<int> opt1{8};
    EXPECT_FALSE(!opt1);

    const Optional<int> opt2{};
    EXPECT_TRUE(!opt2);

    Optional<int> opt3{8};
    EXPECT_FALSE(!opt3);

    Optional<int> opt4{};
    EXPECT_TRUE(!opt4);

    EXPECT_FALSE(!Optional<int>{4});
    EXPECT_TRUE(!Optional<int>{});
    const Optional<int> opt5{4};
    EXPECT_FALSE(!std::move(opt5));
    const Optional<int> opt6{};
    EXPECT_TRUE(!std::move(opt6));
}

TEST(OptionalTest, OperatorEqualTo) {
    Optional<int> opt1{5};
    Optional<int> opt2{5};

    EXPECT_TRUE(opt1 == opt2);

    Optional<int> opt3{};
    Optional<int> opt4{};

    EXPECT_TRUE(opt3 == opt4);

    Optional<int> opt5{};
    Optional<int> opt6{8};

    EXPECT_FALSE(opt5 == opt6);
    EXPECT_FALSE(opt6 == opt5);

    EXPECT_FALSE(opt6 == opt1);

    EXPECT_TRUE(opt1 == opt1);
    EXPECT_TRUE(opt3 == opt3);
}

TEST(OptionalTest, OperatorNotEqualTo) {
    Optional<int> opt1{5};
    Optional<int> opt2{5};

    EXPECT_FALSE(opt1 != opt2);

    Optional<int> opt3{};
    Optional<int> opt4{};

    EXPECT_FALSE(opt3 != opt4);

    Optional<int> opt5{};
    Optional<int> opt6{8};

    EXPECT_TRUE(opt5 != opt6);
    EXPECT_TRUE(opt6 != opt5);

    EXPECT_TRUE(opt6 != opt1);

    EXPECT_FALSE(opt1 != opt1);
    EXPECT_FALSE(opt3 != opt3);
}

TEST(OptionalTest, OperatorLessThan) {
    Optional<int> opt1{1};
    Optional<int> opt2{2};
    Optional<int> opt3{};
    Optional<int> opt4{};

    EXPECT_TRUE(opt1 < opt2);
    EXPECT_FALSE(opt2 < opt1);

    EXPECT_FALSE(opt1 < opt1);
    EXPECT_FALSE(opt3 < opt3);

    EXPECT_FALSE(opt3 < opt4);
    EXPECT_TRUE(opt3 < opt2);
    EXPECT_FALSE(opt1 < opt3);
}

TEST(OptionalTest, OperatorGreaterThan) {
    Optional<int> opt1{1};
    Optional<int> opt2{2};
    Optional<int> opt3{};
    Optional<int> opt4{};

    EXPECT_FALSE(opt1 > opt2);
    EXPECT_TRUE(opt2 > opt1);

    EXPECT_FALSE(opt1 > opt1);
    EXPECT_FALSE(opt3 > opt3);

    EXPECT_FALSE(opt3 > opt4);
    EXPECT_FALSE(opt3 > opt2);
    EXPECT_TRUE(opt1 > opt3);
}

TEST(OptionalTest, OperatorLessThanOrEqualTo) {
    Optional<int> opt1{1};
    Optional<int> opt2{2};
    Optional<int> opt3{};
    Optional<int> opt4{};
    Optional<int> opt5{2};

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

TEST(OptionalTest, OperatorGreaterThanOrEqualTo) {
    Optional<int> opt1{1};
    Optional<int> opt2{2};
    Optional<int> opt3{};
    Optional<int> opt4{};
    Optional<int> opt5{2};

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

TEST(OptionalTest, OperatorEqualToNoneT) {
    Optional<int> opt1{7};
    Optional<int> opt2{};

    EXPECT_FALSE(opt1 == none);
    EXPECT_TRUE(opt2 == none);
    EXPECT_FALSE(none == opt1);
    EXPECT_TRUE(none == opt2);

    opt2 = std::move(opt1);

    EXPECT_FALSE(opt2 == none);
    EXPECT_TRUE(opt1 == none);
    EXPECT_FALSE(none == opt2);
    EXPECT_TRUE(none == opt1);
}

TEST(OptionalTest, OperatorNotEqualToNoneT) {
    Optional<int> opt1{7};
    Optional<int> opt2{};

    EXPECT_TRUE(opt1 != none);
    EXPECT_FALSE(opt2 != none);
    EXPECT_TRUE(none != opt1);
    EXPECT_FALSE(none != opt2);

    opt2 = std::move(opt1);

    EXPECT_TRUE(opt2 != none);
    EXPECT_FALSE(opt1 != none);
    EXPECT_TRUE(none != opt2);
    EXPECT_FALSE(none != opt1);
}

TEST(OptionalTest, GetConstFreeFunction) {
    int i = 7;
    const Optional<int> opt(i);

    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(7, get(opt));
}

TEST(OptionalTest, GetFreeFunction) {
    int i = 7;
    Optional<int> opt(i);

    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(7, get(opt));

    get(opt) = 3;
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(3, get(opt));
}

TEST(OptionalTest, ConstGetFromPointerFreeFunction) {
    const Optional<int> opt{4};
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(4, *(get(&opt)));
}

TEST(OptionalTest, GetFromPointerFreeFunction) {
    Optional<int> opt{4};
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(4, *(get(&opt)));

    *(get(&opt)) = 5;
    EXPECT_EQ(5, opt.get());
}

TEST(OptionalTest, ConstGetPointerFreeFunction) {
    const Optional<int> opt{8};
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(8, *get_pointer(opt));
}

TEST(OptionalTest, GetPointerFreeFunction) {
    Optional<int> opt{8};
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(8, *get_pointer(opt));

    *get_pointer(opt) = 4;
    ASSERT_TRUE(bool(opt));
    EXPECT_EQ(4, opt.get());
}

TEST(OptionalTest, Swap) {
    using std::swap;
    Optional<int> opt1{};
    Optional<int> opt2{};
    Optional<int> opt3{6};
    Optional<int> opt4{32};

    swap(opt1, opt2);
    EXPECT_FALSE(bool(opt1));
    EXPECT_FALSE(bool(opt2));

    swap(opt3, opt4);
    EXPECT_TRUE(bool(opt3));
    EXPECT_TRUE(bool(opt3));
    EXPECT_EQ(32, opt3.get());
    EXPECT_EQ(6, opt4.get());

    swap(opt3, opt2);
    EXPECT_FALSE(bool(opt3));
    EXPECT_TRUE(bool(opt2));
    EXPECT_EQ(32, opt2.get());

    swap(opt1, opt4);
    EXPECT_FALSE(bool(opt4));
    EXPECT_TRUE(bool(opt1));
    EXPECT_EQ(6, opt1.get());
}
