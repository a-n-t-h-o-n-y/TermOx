#include <termox/painter/brush.hpp>
#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/trait.hpp>

#include <gtest/gtest.h>

#include <string>

using ox::background;
using ox::Brush;
using ox::Color;
using ox::foreground;
using ox::Glyph;
using ox::Trait;

std::string foo(const Glyph& g) { return g.str(); }

TEST(GlyphTest, StringConstructor)
{
    EXPECT_NO_THROW(Glyph("H"));
    EXPECT_NO_THROW(Glyph("5"));
    EXPECT_NO_THROW(Glyph('j'));
    const char cp[] = "₹";
    EXPECT_NO_THROW(Glyph{cp});
    EXPECT_NO_THROW(Glyph("ↈ"));
}

TEST(GlyphTest, StringAndTraitsConstructor)
{
    EXPECT_NO_THROW(Glyph("#", Trait::Bold));
    EXPECT_NO_THROW(Glyph("1", bg(Color::Blue), Trait::Underline, Trait::Bold,
                          Trait::Underline));
    EXPECT_NO_THROW(Glyph("ޒ", Trait::Standout));
}

TEST(GlyphTest, Symbol)
{
    Glyph glf_1("H");
    Glyph glf_2("ↈ");
    Glyph glf_3("6");

    EXPECT_EQ("H", glf_1.str());
    EXPECT_EQ("ↈ", glf_2.str());
    EXPECT_EQ("6", glf_3.str());
}

TEST(GlyphTest, SetSymbol)
{
    Glyph g1 = "ੴ";
    Glyph g2 = {"ആ", Trait::Italic, bg(Color::Green)};
    Glyph g3{"g", fg(Color::Red), Trait::Dim};

    EXPECT_EQ("ੴ", g1.str());
    EXPECT_EQ("ആ", g2.str());
    EXPECT_EQ("g", g3.str());

    g1.set_symbol(" ");
    g2.set_symbol("ᆅ");
    g3.set_symbol("ᛥ");

    EXPECT_EQ(" ", g1.str());
    EXPECT_EQ("ᆅ", g2.str());
    EXPECT_EQ("ᛥ", g3.str());
}

TEST(GlyphTest, SetBrush)
{
    Glyph g1{"ៀ"};
    Glyph g2("ᯣ", Trait::Italic, bg(Color::White));

    EXPECT_EQ(Brush{}, g1.brush());
    EXPECT_EQ((Brush{Trait::Italic, bg(Color::White)}), g2.brush());

    g2.brush().foreground = Color::Yellow;
    g2.brush().add_traits(Trait::Standout);

    EXPECT_EQ((Brush{Trait::Standout, Trait::Italic, bg(Color::White),
                     fg(Color::Yellow)}),
              g2.brush());
}
