#include <painter/glyph.hpp>
#include <painter/glyph_string.hpp>
#include <painter/trait.hpp>

#include <gtest/gtest.h>

TEST(GlyphStringTest, GlyphConstructor)
{
    ox::Glyph g1{"⎔", ox::Trait::Bold};
    ox::Glyph_string gs1(g1);

    ox::Glyph g2{"₹", ox::Trait::Bold};
    ox::Glyph_string gs2(g2, ox::Trait::Standout, ox::bg(ox::Color::Black));

    EXPECT_EQ(gs1[0], g1);

    EXPECT_EQ(gs2[0], (ox::Glyph("₹", ox::Trait::Bold, ox::bg(ox::Color::Black),
                                 ox::Trait::Standout)));
}

TEST(GlyphStringTest, GlyphInitializerListConstructor)
{
    ox::Glyph g0{"⎄"};
    ox::Glyph g1{"⎅", ox::Trait::Bold, ox::fg(ox::Color::Yellow)};
    ox::Glyph g2{"A", ox::bg(ox::Color::Blue)};
    ox::Glyph g3{"⎆", ox::Trait::Standout, ox::Trait::Italic};
    ox::Glyph g4{"⎈", ox::Trait::Bold, ox::Trait::Bold};

    ox::Glyph_string gs({g0, g1, g2, g3, g4, {"k", ox::Trait::Italic}},
                        ox::bg(ox::Color::Red));

    EXPECT_EQ(gs[0], (ox::Glyph{"⎄", ox::bg(ox::Color::Red)}));

    EXPECT_EQ(gs[1], (ox::Glyph{"⎅", ox::bg(ox::Color::Red), ox::Trait::Bold,
                                ox::fg(ox::Color::Yellow)}));

    EXPECT_EQ(gs[2], (ox::Glyph{"A", ox::bg(ox::Color::Red)}));

    EXPECT_EQ(gs[3], (ox::Glyph{"⎆", ox::bg(ox::Color::Red),
                                ox::Trait::Standout, ox::Trait::Italic}));

    EXPECT_EQ(gs[4], (ox::Glyph{"⎈", ox::bg(ox::Color::Red), ox::Trait::Bold}));

    EXPECT_EQ(gs[5],
              (ox::Glyph{"k", ox::bg(ox::Color::Red), ox::Trait::Italic}));
}

TEST(GlyphStringTest, CharConstructor)
{
    ox::Glyph_string gs1{'g', ox::Trait::Bold, ox::bg(ox::Color::Blue)};

    ox::Glyph_string gs2 = 'h';

    char c = '4';
    ox::Glyph_string gs3(c, ox::Trait::Dim);

    EXPECT_EQ(gs1[0],
              (ox::Glyph{"g", ox::bg(ox::Color::Blue), ox::Trait::Bold}));

    EXPECT_EQ(gs2[0], (ox::Glyph{"h"}));

    EXPECT_EQ(gs3[0], (ox::Glyph{"4", ox::Trait::Dim}));
}

TEST(GlyphStringTest, ConstCharPointerConstructor)
{
    ox::Glyph_string gs1{"hello!", ox::Trait::Bold};
    ox::Glyph_string gs2("⎉ab_⍇⍈ ⍼");
    ox::Glyph_string gs3 = "* ⎲⎳";
    ox::Glyph_string gs4{""};
    ox::Glyph_string gs5{"⎋∇ Ⅷ₷", ox::fg(ox::Color::Green), ox::Trait::Dim};
    const char str[6]    = {'a', 'b', 'C', 'd', 'E', '\0'};
    ox::Glyph_string gs6 = {str, ox::Trait::Italic};

    EXPECT_EQ(gs1[1], (ox::Glyph("e", ox::Trait::Bold)));

    EXPECT_EQ(gs2[3], (ox::Glyph("_")));
    EXPECT_EQ(gs2[5], (ox::Glyph("⍈")));
    EXPECT_EQ(gs2[6], (ox::Glyph(" ")));
    EXPECT_EQ(8, gs2.size());

    EXPECT_EQ(gs3[3], (ox::Glyph("⎳")));

    EXPECT_EQ(0, gs4.size());

    ASSERT_EQ(5, gs5.size());
    EXPECT_EQ(gs5[3],
              (ox::Glyph{"Ⅷ", ox::Trait::Dim, ox::fg(ox::Color::Green)}));

    ASSERT_EQ(5, gs6.size());
    EXPECT_EQ(gs6[2], (ox::Glyph{"C", ox::Trait::Italic}));
    EXPECT_EQ(gs6[4], (ox::Glyph{"E", ox::Trait::Italic}));
}

TEST(GlyphStringTest, StdStringConstructor)
{
    ox::Glyph_string gs1{std::string("hello!"), ox::Trait::Bold};
    ox::Glyph_string gs2(std::string{"⎉ab_⍇⍈ ⍼"});
    ox::Glyph_string gs3 = std::string("* ⎲⎳");
    ox::Glyph_string gs4{std::string("")};
    ox::Glyph_string gs5{std::string{"⎋∇ Ⅷ₷"}, ox::fg(ox::Color::Green),
                         ox::Trait::Dim};
    std::string str      = "abCdE";
    ox::Glyph_string gs6 = {str, ox::Trait::Italic};

    EXPECT_EQ(gs1[1], (ox::Glyph("e", ox::Trait::Bold)));

    EXPECT_EQ(gs2[3], (ox::Glyph("_")));
    EXPECT_EQ(gs2[5], (ox::Glyph("⍈")));
    EXPECT_EQ(gs2[6], (ox::Glyph(" ")));
    EXPECT_EQ(8, gs2.size());

    EXPECT_EQ(gs3[3], (ox::Glyph("⎳")));

    EXPECT_EQ(0, gs4.size());

    ASSERT_EQ(5, gs5.size());
    EXPECT_EQ(gs5[3],
              (ox::Glyph{"Ⅷ", ox::Trait::Dim, ox::fg(ox::Color::Green)}));

    ASSERT_EQ(5, gs6.size());
    EXPECT_EQ(gs6[2], (ox::Glyph{"C", ox::Trait::Italic}));
    EXPECT_EQ(gs6[4], (ox::Glyph{"E", ox::Trait::Italic}));
}

TEST(GlyphStringTest, EqualityOperator)
{
    ox::Glyph_string gs1{"⁗ 7₱", ox::Trait::Bold, ox::bg(ox::Color::Red),
                         ox::Trait::Italic};
    ox::Glyph_string gs2{"⁗ 7₱", ox::Trait::Italic, ox::Trait::Bold,
                         ox::bg(ox::Color::Red)};

    EXPECT_TRUE(gs1 == gs2);
}

TEST(GlyphStringTest, Str)
{
    ox::Glyph_string gs1{"⁗ 7₱", ox::Trait::Bold, ox::bg(ox::Color::Red),
                         ox::Trait::Italic};

    ox::Glyph_string gs2(std::string{"⎉ab_⍇⍈ ⍼"});

    ox::Glyph_string gs3{"", ox::Trait::Bold, ox::bg(ox::Color::Yellow)};

    EXPECT_EQ(gs1.str(), "⁗ 7₱");
    EXPECT_EQ(gs2.str(), "⎉ab_⍇⍈ ⍼");
    EXPECT_EQ(gs3.str(), "");

    EXPECT_EQ(6, gs2.str().find("⍇"));
}

TEST(GlyphStringTest, OperatorPlus)
{
    ox::Glyph_string gs1{"⁗ 7₱", ox::Trait::Bold, ox::bg(ox::Color::Red),
                         ox::Trait::Italic};

    ox::Glyph_string gs2("⎉ab_⍇⍈ ⍼", ox::Trait::Italic);

    ox::Glyph_string gs = gs1 + gs2;

    EXPECT_EQ(gs, (ox::Glyph_string{{gs1[0], gs1[1], gs1[2], gs1[3], gs2[0],
                                     gs2[1], gs2[2], gs2[3], gs2[4], gs2[5],
                                     gs2[6], gs2[7]}}));
    EXPECT_EQ(gs[6], (ox::Glyph("b", ox::Trait::Italic)));
}
