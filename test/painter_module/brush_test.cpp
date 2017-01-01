#include <painter_module/attribute.hpp>
#include <painter_module/brush.hpp>
#include <painter_module/color.hpp>

#include <gtest/gtest.h>

using twf::Attribute;
using twf::background;
using twf::Brush;
using twf::Color;
using twf::foreground;

TEST(BrushTest, Constructor) {
    Brush b1;
    Brush b2(Attribute::Bold);
    Brush b3(background(Color::Black), foreground(Color::White),
             Attribute::Italic);
    Brush b4{Attribute::Underline, Attribute::Italic, background(Color::Red)};

    EXPECT_FALSE(bool(b1.background_color()));
    EXPECT_FALSE(bool(b1.foreground_color()));
    EXPECT_EQ(0, b1.attributes().size());

    ASSERT_EQ(1, b2.attributes().size());
    EXPECT_EQ(Attribute::Bold, b2.attributes()[0]);

    ASSERT_TRUE(bool(b3.background_color()));
    EXPECT_EQ(Color::Black, *b3.background_color());
    ASSERT_TRUE(bool(b3.foreground_color()));
    EXPECT_EQ(Color::White, *b3.foreground_color());
    ASSERT_EQ(1, b3.attributes().size());
    EXPECT_EQ(Attribute::Italic, b3.attributes()[0]);

    ASSERT_TRUE(bool(b4.background_color()));
    EXPECT_FALSE(bool(b4.foreground_color()));
    EXPECT_EQ(Color::Red, *b4.background_color());
    EXPECT_EQ(2, b4.attributes().size());
}

TEST(BrushTest, RemoveAttribute) {
    Brush b(Attribute::Bold, foreground(Color::Green), Attribute::Italic,
            Attribute::Bold, Attribute::Underline);
    EXPECT_EQ(3, b.attributes().size());
    b.remove_attribute(Attribute::Italic);
    EXPECT_EQ(2, b.attributes().size());
    b.remove_attribute(Attribute::Italic);
    EXPECT_EQ(2, b.attributes().size());
    b.remove_attribute(Attribute::Bold);
    EXPECT_EQ(1, b.attributes().size());
    b.remove_attribute(Attribute::Underline);
    EXPECT_EQ(0, b.attributes().size());
    b.remove_attribute(Attribute::Standout);
    EXPECT_EQ(0, b.attributes().size());
    EXPECT_FALSE(bool(b.background_color()));
    ASSERT_TRUE(bool(b.foreground_color()));
    EXPECT_EQ(Color::Green, *b.foreground_color());
}
