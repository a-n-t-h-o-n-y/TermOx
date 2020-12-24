#include <painter/brush.hpp>
#include <painter/color.hpp>
#include <painter/trait.hpp>

#include <gtest/gtest.h>

using ox::background;
using ox::Brush;
using ox::Color;
using ox::foreground;
using ox::Trait;

TEST(BrushTest, Constructor)
{
    Brush b1;
    Brush b2(Trait::Bold);
    Brush b3(ng(Color::Black), fg(Color::White), Trait::Italic);
    Brush b4{Trait::Underline, Trait::Italic, bg(Color::Red)};

    EXPECT_FALSE(bool(b1.background));
    EXPECT_FALSE(bool(b1.foreground));
    EXPECT_EQ(0, b1.traits().size());

    ASSERT_EQ(1, b2.traits().size());
    EXPECT_EQ(Trait::Bold, b2.traits()[0]);

    ASSERT_TRUE(bool(b3.background));
    EXPECT_EQ(Color::Black, *b3.background);
    ASSERT_TRUE(bool(b3.foreground));
    EXPECT_EQ(Color::White, *b3.foreground);
    ASSERT_EQ(1, b3.traits().size());
    EXPECT_EQ(Trait::Italic, b3.traits()[0]);

    ASSERT_TRUE(bool(b4.background));
    EXPECT_FALSE(bool(b4.foreground));
    EXPECT_EQ(Color::Red, *b4.background);
    EXPECT_EQ(2, b4.traits().size());
}

TEST(BrushTest, RemoveTrait)
{
    Brush b(Trait::Bold, fg(Color::Green), Trait::Italic, Trait::Bold,
            Trait::Underline);
    EXPECT_EQ(3, b.traits().size());
    b.remove_traits(Trait::Italic);
    EXPECT_EQ(2, b.traits().size());
    b.remove_traits(Trait::Italic);
    EXPECT_EQ(2, b.traits().size());
    b.remove_traits(Trait::Bold);
    EXPECT_EQ(1, b.traits().size());
    b.remove_traits(Trait::Underline);
    EXPECT_EQ(0, b.traits().size());
    b.remove_traits(Trait::Standout);
    EXPECT_EQ(0, b.traits().size());
    EXPECT_FALSE(bool(b.background));
    ASSERT_TRUE(bool(b.foreground));
    EXPECT_EQ(Color::Green, *b.foreground);
}
