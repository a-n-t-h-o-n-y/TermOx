#include <painter_module/brush.hpp>
#include <painter_module/color.hpp>
#include <painter_module/attribute.hpp>

#include <gtest/gtest.h>

TEST(BrushTest, Constructor) {
	mcurses::Brush b1;
	mcurses::Brush b2(mcurses::Attribute::Bold);
	mcurses::Brush b3(mcurses::background(mcurses::Color::Black), mcurses::foreground(mcurses::Color::White), mcurses::Attribute::Italic);
	mcurses::Brush b4{mcurses::Attribute::Underline, mcurses::Attribute::Italic, mcurses::background(mcurses::Color::Red)};

	EXPECT_FALSE(bool(b1.background_color()));
	EXPECT_FALSE(bool(b1.foreground_color()));
	EXPECT_EQ(0, b1.attributes().size());

	ASSERT_EQ(1, b2.attributes().size());
	EXPECT_EQ(mcurses::Attribute::Bold, b2.attributes()[0]);

	ASSERT_TRUE(bool(b3.background_color()));
	EXPECT_EQ(mcurses::Color::Black, *b3.background_color());
	ASSERT_TRUE(bool(b3.foreground_color()));
	EXPECT_EQ(mcurses::Color::White, *b3.foreground_color());
	ASSERT_EQ(1, b3.attributes().size());
	EXPECT_EQ(mcurses::Attribute::Italic, b3.attributes()[0]);

	ASSERT_TRUE(bool(b4.background_color()));
	EXPECT_FALSE(bool(b4.foreground_color()));
	EXPECT_EQ(mcurses::Color::Red, *b4.background_color());
	EXPECT_EQ(2, b4.attributes().size());
}

TEST(BrushTest, RemoveAttribute) {
	mcurses::Brush b(mcurses::Attribute::Bold, mcurses::foreground(mcurses::Color::Green), mcurses::Attribute::Italic, mcurses::Attribute::Bold, mcurses::Attribute::Underline);
	EXPECT_EQ(3, b.attributes().size());
	b.remove_attribute(mcurses::Attribute::Italic);
	EXPECT_EQ(2, b.attributes().size());
	b.remove_attribute(mcurses::Attribute::Italic);
	EXPECT_EQ(2, b.attributes().size());
	b.remove_attribute(mcurses::Attribute::Bold);
	EXPECT_EQ(1, b.attributes().size());
	b.remove_attribute(mcurses::Attribute::Underline);
	EXPECT_EQ(0, b.attributes().size());
	b.remove_attribute(mcurses::Attribute::Standout);
	EXPECT_EQ(0, b.attributes().size());
	EXPECT_FALSE(bool(b.background_color()));
	ASSERT_TRUE(bool(b.foreground_color()));
	EXPECT_EQ(mcurses::Color::Green, *b.foreground_color());
}
