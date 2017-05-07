#include <painter_module/glyph_string.hpp>
#include <painter_module/glyph.hpp>
#include <painter_module/attribute.hpp>

#include <gtest/gtest.h>

TEST(GlyphStringTest, GlyphConstructor) {
    twf::Glyph g1{"⎔", twf::Attribute::Bold};
    twf::Glyph_string gs1(g1);

    twf::Glyph g2{"₹", twf::Attribute::Bold};
    twf::Glyph_string gs2(g2, twf::Attribute::Standout,
                              twf::background(twf::Color::Black));

    EXPECT_EQ(gs1[0], g1);

    EXPECT_EQ(gs2[0],
              (twf::Glyph("₹", twf::Attribute::Bold,
                              twf::background(twf::Color::Black),
                              twf::Attribute::Standout)));
}

TEST(GlyphStringTest, GlyphInitializerListConstructor) {
    twf::Glyph g0{"⎄"};
    twf::Glyph g1{"⎅", twf::Attribute::Bold,
                      twf::foreground(twf::Color::Yellow)};
    twf::Glyph g2{"A", twf::background(twf::Color::Blue)};
    twf::Glyph g3{"⎆", twf::Attribute::Standout,
                      twf::Attribute::Italic};
    twf::Glyph g4{"⎈", twf::Attribute::Bold, twf::Attribute::Bold};

    twf::Glyph_string gs(
        {g0, g1, g2, g3, g4, {"k", twf::Attribute::Italic}},
        twf::background(twf::Color::Red));

    EXPECT_EQ(gs[0],
              (twf::Glyph{"⎄", twf::background(twf::Color::Red)}));

    EXPECT_EQ(gs[1],
              (twf::Glyph{"⎅", twf::background(twf::Color::Red),
                              twf::Attribute::Bold,
                              twf::foreground(twf::Color::Yellow)}));

    EXPECT_EQ(gs[2],
              (twf::Glyph{"A", twf::background(twf::Color::Red)}));

    EXPECT_EQ(gs[3],
              (twf::Glyph{"⎆", twf::background(twf::Color::Red),
                              twf::Attribute::Standout,
                              twf::Attribute::Italic}));

    EXPECT_EQ(gs[4],
              (twf::Glyph{"⎈", twf::background(twf::Color::Red),
                              twf::Attribute::Bold}));

    EXPECT_EQ(gs[5],
              (twf::Glyph{"k", twf::background(twf::Color::Red),
                              twf::Attribute::Italic}));
}

TEST(GlyphStringTest, CharConstructor) {
    twf::Glyph_string gs1{'g', twf::Attribute::Bold,
                              twf::background(twf::Color::Blue)};

    twf::Glyph_string gs2 = 'h';

    char c = '4';
    twf::Glyph_string gs3(c, twf::Attribute::Dim);

    EXPECT_EQ(gs1[0],
              (twf::Glyph{"g", twf::background(twf::Color::Blue),
                              twf::Attribute::Bold}));

    EXPECT_EQ(gs2[0], (twf::Glyph{"h"}));

    EXPECT_EQ(gs3[0], (twf::Glyph{"4", twf::Attribute::Dim}));
}

TEST(GlyphStringTest, ConstCharPointerConstructor) {
    twf::Glyph_string gs1{"hello!", twf::Attribute::Bold};
    twf::Glyph_string gs2("⎉ab_⍇⍈ ⍼");
    twf::Glyph_string gs3 = "* ⎲⎳";
    twf::Glyph_string gs4{""};
    twf::Glyph_string gs5{"⎋∇ Ⅷ₷",
                              twf::foreground(twf::Color::Green),
                              twf::Attribute::Dim};
    const char str[6] = {'a', 'b', 'C', 'd', 'E', '\0'};
    twf::Glyph_string gs6 = {str, twf::Attribute::Italic};

    EXPECT_EQ(gs1[1], (twf::Glyph("e", twf::Attribute::Bold)));

    EXPECT_EQ(gs2[3], (twf::Glyph("_")));
    EXPECT_EQ(gs2[5], (twf::Glyph("⍈")));
    EXPECT_EQ(gs2[6], (twf::Glyph(" ")));
    EXPECT_EQ(8, gs2.size());

    EXPECT_EQ(gs3[3], (twf::Glyph("⎳")));

    EXPECT_EQ(0, gs4.size());

    ASSERT_EQ(5, gs5.size());
    EXPECT_EQ(gs5[3],
              (twf::Glyph{"Ⅷ", twf::Attribute::Dim,
                              twf::foreground(twf::Color::Green)}));

    ASSERT_EQ(5, gs6.size());
    EXPECT_EQ(gs6[2], (twf::Glyph{"C", twf::Attribute::Italic}));
    EXPECT_EQ(gs6[4], (twf::Glyph{"E", twf::Attribute::Italic}));
}

TEST(GlyphStringTest, StdStringConstructor) {
    twf::Glyph_string gs1{std::string("hello!"), twf::Attribute::Bold};
    twf::Glyph_string gs2(std::string{"⎉ab_⍇⍈ ⍼"});
    twf::Glyph_string gs3 = std::string("* ⎲⎳");
    twf::Glyph_string gs4{std::string("")};
    twf::Glyph_string gs5{std::string{"⎋∇ Ⅷ₷"},
                              twf::foreground(twf::Color::Green),
                              twf::Attribute::Dim};
    std::string str = "abCdE";
    twf::Glyph_string gs6 = {str, twf::Attribute::Italic};

    EXPECT_EQ(gs1[1], (twf::Glyph("e", twf::Attribute::Bold)));

    EXPECT_EQ(gs2[3], (twf::Glyph("_")));
    EXPECT_EQ(gs2[5], (twf::Glyph("⍈")));
    EXPECT_EQ(gs2[6], (twf::Glyph(" ")));
    EXPECT_EQ(8, gs2.size());

    EXPECT_EQ(gs3[3], (twf::Glyph("⎳")));

    EXPECT_EQ(0, gs4.size());

    ASSERT_EQ(5, gs5.size());
    EXPECT_EQ(gs5[3],
              (twf::Glyph{"Ⅷ", twf::Attribute::Dim,
                              twf::foreground(twf::Color::Green)}));

    ASSERT_EQ(5, gs6.size());
    EXPECT_EQ(gs6[2], (twf::Glyph{"C", twf::Attribute::Italic}));
    EXPECT_EQ(gs6[4], (twf::Glyph{"E", twf::Attribute::Italic}));
}

TEST(GlyphStringTest, EqualityOperator) {
    twf::Glyph_string gs1{"⁗ 7₱", twf::Attribute::Bold,
                              twf::background(twf::Color::Red),
                              twf::Attribute::Italic};
    twf::Glyph_string gs2{"⁗ 7₱", twf::Attribute::Italic,
                              twf::Attribute::Bold,
                              twf::background(twf::Color::Red)};

    EXPECT_TRUE(gs1 == gs2);
}

TEST(GlyphStringTest, Str) {
    twf::Glyph_string gs1{"⁗ 7₱", twf::Attribute::Bold,
                              twf::background(twf::Color::Red),
                              twf::Attribute::Italic};

    twf::Glyph_string gs2(std::string{"⎉ab_⍇⍈ ⍼"});

    twf::Glyph_string gs3{"", twf::Attribute::Bold,
                              twf::background(twf::Color::Yellow)};

    EXPECT_EQ(gs1.str(), "⁗ 7₱");
    EXPECT_EQ(gs2.str(), "⎉ab_⍇⍈ ⍼");
    EXPECT_EQ(gs3.str(), "");

    EXPECT_EQ(6, gs2.str().find("⍇"));
}

TEST(GlyphStringTest, OperatorPlus) {
    twf::Glyph_string gs1{"⁗ 7₱", twf::Attribute::Bold,
                              twf::background(twf::Color::Red),
                              twf::Attribute::Italic};

    twf::Glyph_string gs2("⎉ab_⍇⍈ ⍼", twf::Attribute::Italic);

    twf::Glyph_string gs = gs1 + gs2;

    EXPECT_EQ(gs, (twf::Glyph_string{{gs1[0], gs1[1], gs1[2], gs1[3],
                                          gs2[0], gs2[1], gs2[2], gs2[3],
                                          gs2[4], gs2[5], gs2[6], gs2[7]}}));
    EXPECT_EQ(gs[6], (twf::Glyph("b", twf::Attribute::Italic)));
}
