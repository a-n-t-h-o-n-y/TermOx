#include <painter/glyph.hpp>
#include <painter/glyph_string.hpp>
#include <painter/trait.hpp>

#include <gtest/gtest.h>

TEST(GlyphStringTest, GlyphConstructor)
{
    cppurses::Glyph g1{"⎔", cppurses::Trait::Bold};
    cppurses::Glyph_string gs1(g1);

    cppurses::Glyph g2{"₹", cppurses::Trait::Bold};
    cppurses::Glyph_string gs2(g2, cppurses::Trait::Standout,
                               cppurses::background(cppurses::Color::Black));

    EXPECT_EQ(gs1[0], g1);

    EXPECT_EQ(gs2[0],
              (cppurses::Glyph("₹", cppurses::Trait::Bold,
                               cppurses::background(cppurses::Color::Black),
                               cppurses::Trait::Standout)));
}

TEST(GlyphStringTest, GlyphInitializerListConstructor)
{
    cppurses::Glyph g0{"⎄"};
    cppurses::Glyph g1{"⎅", cppurses::Trait::Bold,
                       cppurses::foreground(cppurses::Color::Yellow)};
    cppurses::Glyph g2{"A", cppurses::background(cppurses::Color::Blue)};
    cppurses::Glyph g3{"⎆", cppurses::Trait::Standout, cppurses::Trait::Italic};
    cppurses::Glyph g4{"⎈", cppurses::Trait::Bold, cppurses::Trait::Bold};

    cppurses::Glyph_string gs(
        {g0, g1, g2, g3, g4, {"k", cppurses::Trait::Italic}},
        cppurses::background(cppurses::Color::Red));

    EXPECT_EQ(gs[0], (cppurses::Glyph{
                         "⎄", cppurses::background(cppurses::Color::Red)}));

    EXPECT_EQ(gs[1],
              (cppurses::Glyph{"⎅", cppurses::background(cppurses::Color::Red),
                               cppurses::Trait::Bold,
                               cppurses::foreground(cppurses::Color::Yellow)}));

    EXPECT_EQ(gs[2], (cppurses::Glyph{
                         "A", cppurses::background(cppurses::Color::Red)}));

    EXPECT_EQ(gs[3], (cppurses::Glyph{
                         "⎆", cppurses::background(cppurses::Color::Red),
                         cppurses::Trait::Standout, cppurses::Trait::Italic}));

    EXPECT_EQ(gs[4],
              (cppurses::Glyph{"⎈", cppurses::background(cppurses::Color::Red),
                               cppurses::Trait::Bold}));

    EXPECT_EQ(gs[5],
              (cppurses::Glyph{"k", cppurses::background(cppurses::Color::Red),
                               cppurses::Trait::Italic}));
}

TEST(GlyphStringTest, CharConstructor)
{
    cppurses::Glyph_string gs1{'g', cppurses::Trait::Bold,
                               cppurses::background(cppurses::Color::Blue)};

    cppurses::Glyph_string gs2 = 'h';

    char c = '4';
    cppurses::Glyph_string gs3(c, cppurses::Trait::Dim);

    EXPECT_EQ(gs1[0],
              (cppurses::Glyph{"g", cppurses::background(cppurses::Color::Blue),
                               cppurses::Trait::Bold}));

    EXPECT_EQ(gs2[0], (cppurses::Glyph{"h"}));

    EXPECT_EQ(gs3[0], (cppurses::Glyph{"4", cppurses::Trait::Dim}));
}

TEST(GlyphStringTest, ConstCharPointerConstructor)
{
    cppurses::Glyph_string gs1{"hello!", cppurses::Trait::Bold};
    cppurses::Glyph_string gs2("⎉ab_⍇⍈ ⍼");
    cppurses::Glyph_string gs3 = "* ⎲⎳";
    cppurses::Glyph_string gs4{""};
    cppurses::Glyph_string gs5{"⎋∇ Ⅷ₷",
                               cppurses::foreground(cppurses::Color::Green),
                               cppurses::Trait::Dim};
    const char str[6]          = {'a', 'b', 'C', 'd', 'E', '\0'};
    cppurses::Glyph_string gs6 = {str, cppurses::Trait::Italic};

    EXPECT_EQ(gs1[1], (cppurses::Glyph("e", cppurses::Trait::Bold)));

    EXPECT_EQ(gs2[3], (cppurses::Glyph("_")));
    EXPECT_EQ(gs2[5], (cppurses::Glyph("⍈")));
    EXPECT_EQ(gs2[6], (cppurses::Glyph(" ")));
    EXPECT_EQ(8, gs2.size());

    EXPECT_EQ(gs3[3], (cppurses::Glyph("⎳")));

    EXPECT_EQ(0, gs4.size());

    ASSERT_EQ(5, gs5.size());
    EXPECT_EQ(gs5[3],
              (cppurses::Glyph{"Ⅷ", cppurses::Trait::Dim,
                               cppurses::foreground(cppurses::Color::Green)}));

    ASSERT_EQ(5, gs6.size());
    EXPECT_EQ(gs6[2], (cppurses::Glyph{"C", cppurses::Trait::Italic}));
    EXPECT_EQ(gs6[4], (cppurses::Glyph{"E", cppurses::Trait::Italic}));
}

TEST(GlyphStringTest, StdStringConstructor)
{
    cppurses::Glyph_string gs1{std::string("hello!"), cppurses::Trait::Bold};
    cppurses::Glyph_string gs2(std::string{"⎉ab_⍇⍈ ⍼"});
    cppurses::Glyph_string gs3 = std::string("* ⎲⎳");
    cppurses::Glyph_string gs4{std::string("")};
    cppurses::Glyph_string gs5{std::string{"⎋∇ Ⅷ₷"},
                               cppurses::foreground(cppurses::Color::Green),
                               cppurses::Trait::Dim};
    std::string str            = "abCdE";
    cppurses::Glyph_string gs6 = {str, cppurses::Trait::Italic};

    EXPECT_EQ(gs1[1], (cppurses::Glyph("e", cppurses::Trait::Bold)));

    EXPECT_EQ(gs2[3], (cppurses::Glyph("_")));
    EXPECT_EQ(gs2[5], (cppurses::Glyph("⍈")));
    EXPECT_EQ(gs2[6], (cppurses::Glyph(" ")));
    EXPECT_EQ(8, gs2.size());

    EXPECT_EQ(gs3[3], (cppurses::Glyph("⎳")));

    EXPECT_EQ(0, gs4.size());

    ASSERT_EQ(5, gs5.size());
    EXPECT_EQ(gs5[3],
              (cppurses::Glyph{"Ⅷ", cppurses::Trait::Dim,
                               cppurses::foreground(cppurses::Color::Green)}));

    ASSERT_EQ(5, gs6.size());
    EXPECT_EQ(gs6[2], (cppurses::Glyph{"C", cppurses::Trait::Italic}));
    EXPECT_EQ(gs6[4], (cppurses::Glyph{"E", cppurses::Trait::Italic}));
}

TEST(GlyphStringTest, EqualityOperator)
{
    cppurses::Glyph_string gs1{"⁗ 7₱", cppurses::Trait::Bold,
                               cppurses::background(cppurses::Color::Red),
                               cppurses::Trait::Italic};
    cppurses::Glyph_string gs2{"⁗ 7₱", cppurses::Trait::Italic,
                               cppurses::Trait::Bold,
                               cppurses::background(cppurses::Color::Red)};

    EXPECT_TRUE(gs1 == gs2);
}

TEST(GlyphStringTest, Str)
{
    cppurses::Glyph_string gs1{"⁗ 7₱", cppurses::Trait::Bold,
                               cppurses::background(cppurses::Color::Red),
                               cppurses::Trait::Italic};

    cppurses::Glyph_string gs2(std::string{"⎉ab_⍇⍈ ⍼"});

    cppurses::Glyph_string gs3{"", cppurses::Trait::Bold,
                               cppurses::background(cppurses::Color::Yellow)};

    EXPECT_EQ(gs1.str(), "⁗ 7₱");
    EXPECT_EQ(gs2.str(), "⎉ab_⍇⍈ ⍼");
    EXPECT_EQ(gs3.str(), "");

    EXPECT_EQ(6, gs2.str().find("⍇"));
}

TEST(GlyphStringTest, OperatorPlus)
{
    cppurses::Glyph_string gs1{"⁗ 7₱", cppurses::Trait::Bold,
                               cppurses::background(cppurses::Color::Red),
                               cppurses::Trait::Italic};

    cppurses::Glyph_string gs2("⎉ab_⍇⍈ ⍼", cppurses::Trait::Italic);

    cppurses::Glyph_string gs = gs1 + gs2;

    EXPECT_EQ(gs, (cppurses::Glyph_string{{gs1[0], gs1[1], gs1[2], gs1[3],
                                           gs2[0], gs2[1], gs2[2], gs2[3],
                                           gs2[4], gs2[5], gs2[6], gs2[7]}}));
    EXPECT_EQ(gs[6], (cppurses::Glyph("b", cppurses::Trait::Italic)));
}
