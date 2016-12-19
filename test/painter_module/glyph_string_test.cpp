#include <painter_module/glyph_string.hpp>
#include <painter_module/glyph.hpp>
#include <painter_module/attribute.hpp>

#include <gtest/gtest.h>

TEST(GlyphStringTest, GlyphConstructor) {
    mcurses::Glyph g1{"⎔", mcurses::Attribute::Bold};
    mcurses::Glyph_string gs1(g1);

    mcurses::Glyph g2{"₹", mcurses::Attribute::Bold};
    mcurses::Glyph_string gs2(g2, mcurses::Attribute::Standout,
                              mcurses::background(mcurses::Color::Black));

    EXPECT_EQ(gs1[0], g1);

    EXPECT_EQ(gs2[0],
              (mcurses::Glyph("₹", mcurses::Attribute::Bold,
                              mcurses::background(mcurses::Color::Black),
                              mcurses::Attribute::Standout)));
}

TEST(GlyphStringTest, GlyphInitializerListConstructor) {
    mcurses::Glyph g0{"⎄"};
    mcurses::Glyph g1{"⎅", mcurses::Attribute::Bold,
                      mcurses::foreground(mcurses::Color::Yellow)};
    mcurses::Glyph g2{"A", mcurses::background(mcurses::Color::Blue)};
    mcurses::Glyph g3{"⎆", mcurses::Attribute::Standout,
                      mcurses::Attribute::Italic};
    mcurses::Glyph g4{"⎈", mcurses::Attribute::Bold, mcurses::Attribute::Bold};

    mcurses::Glyph_string gs(
        {g0, g1, g2, g3, g4, {"k", mcurses::Attribute::Italic}},
        mcurses::background(mcurses::Color::Red));

    EXPECT_EQ(gs[0],
              (mcurses::Glyph{"⎄", mcurses::background(mcurses::Color::Red)}));

    EXPECT_EQ(gs[1],
              (mcurses::Glyph{"⎅", mcurses::background(mcurses::Color::Red),
                              mcurses::Attribute::Bold,
                              mcurses::foreground(mcurses::Color::Yellow)}));

    EXPECT_EQ(gs[2],
              (mcurses::Glyph{"A", mcurses::background(mcurses::Color::Red)}));

    EXPECT_EQ(gs[3],
              (mcurses::Glyph{"⎆", mcurses::background(mcurses::Color::Red),
                              mcurses::Attribute::Standout,
                              mcurses::Attribute::Italic}));

    EXPECT_EQ(gs[4],
              (mcurses::Glyph{"⎈", mcurses::background(mcurses::Color::Red),
                              mcurses::Attribute::Bold}));

    EXPECT_EQ(gs[5],
              (mcurses::Glyph{"k", mcurses::background(mcurses::Color::Red),
                              mcurses::Attribute::Italic}));
}

TEST(GlyphStringTest, CharConstructor) {
    mcurses::Glyph_string gs1{'g', mcurses::Attribute::Bold,
                              mcurses::background(mcurses::Color::Blue)};

    mcurses::Glyph_string gs2 = 'h';

    char c = '4';
    mcurses::Glyph_string gs3(c, mcurses::Attribute::Dim);

    EXPECT_EQ(gs1[0],
              (mcurses::Glyph{"g", mcurses::background(mcurses::Color::Blue),
                              mcurses::Attribute::Bold}));

    EXPECT_EQ(gs2[0], (mcurses::Glyph{"h"}));

    EXPECT_EQ(gs3[0], (mcurses::Glyph{"4", mcurses::Attribute::Dim}));
}

TEST(GlyphStringTest, ConstCharPointerConstructor) {
    mcurses::Glyph_string gs1{"hello!", mcurses::Attribute::Bold};
    mcurses::Glyph_string gs2("⎉ab_⍇⍈ ⍼");
    mcurses::Glyph_string gs3 = "* ⎲⎳";
    mcurses::Glyph_string gs4{""};
    mcurses::Glyph_string gs5{"⎋∇ Ⅷ₷",
                              mcurses::foreground(mcurses::Color::Green),
                              mcurses::Attribute::Dim};
    const char str[6] = {'a', 'b', 'C', 'd', 'E', '\0'};
    mcurses::Glyph_string gs6 = {str, mcurses::Attribute::Italic};

    EXPECT_EQ(gs1[1], (mcurses::Glyph("e", mcurses::Attribute::Bold)));

    EXPECT_EQ(gs2[3], (mcurses::Glyph("_")));
    EXPECT_EQ(gs2[5], (mcurses::Glyph("⍈")));
    EXPECT_EQ(gs2[6], (mcurses::Glyph(" ")));
    EXPECT_EQ(8, gs2.size());

    EXPECT_EQ(gs3[3], (mcurses::Glyph("⎳")));

    EXPECT_EQ(0, gs4.size());

    ASSERT_EQ(5, gs5.size());
    EXPECT_EQ(gs5[3],
              (mcurses::Glyph{"Ⅷ", mcurses::Attribute::Dim,
                              mcurses::foreground(mcurses::Color::Green)}));

    ASSERT_EQ(5, gs6.size());
    EXPECT_EQ(gs6[2], (mcurses::Glyph{"C", mcurses::Attribute::Italic}));
    EXPECT_EQ(gs6[4], (mcurses::Glyph{"E", mcurses::Attribute::Italic}));
}

TEST(GlyphStringTest, StdStringConstructor) {
    mcurses::Glyph_string gs1{std::string("hello!"), mcurses::Attribute::Bold};
    mcurses::Glyph_string gs2(std::string{"⎉ab_⍇⍈ ⍼"});
    mcurses::Glyph_string gs3 = std::string("* ⎲⎳");
    mcurses::Glyph_string gs4{std::string("")};
    mcurses::Glyph_string gs5{std::string{"⎋∇ Ⅷ₷"},
                              mcurses::foreground(mcurses::Color::Green),
                              mcurses::Attribute::Dim};
    std::string str = "abCdE";
    mcurses::Glyph_string gs6 = {str, mcurses::Attribute::Italic};

    EXPECT_EQ(gs1[1], (mcurses::Glyph("e", mcurses::Attribute::Bold)));

    EXPECT_EQ(gs2[3], (mcurses::Glyph("_")));
    EXPECT_EQ(gs2[5], (mcurses::Glyph("⍈")));
    EXPECT_EQ(gs2[6], (mcurses::Glyph(" ")));
    EXPECT_EQ(8, gs2.size());

    EXPECT_EQ(gs3[3], (mcurses::Glyph("⎳")));

    EXPECT_EQ(0, gs4.size());

    ASSERT_EQ(5, gs5.size());
    EXPECT_EQ(gs5[3],
              (mcurses::Glyph{"Ⅷ", mcurses::Attribute::Dim,
                              mcurses::foreground(mcurses::Color::Green)}));

    ASSERT_EQ(5, gs6.size());
    EXPECT_EQ(gs6[2], (mcurses::Glyph{"C", mcurses::Attribute::Italic}));
    EXPECT_EQ(gs6[4], (mcurses::Glyph{"E", mcurses::Attribute::Italic}));
}

TEST(GlyphStringTest, EqualityOperator) {
    mcurses::Glyph_string gs1{"⁗ 7₱", mcurses::Attribute::Bold,
                              mcurses::background(mcurses::Color::Red),
                              mcurses::Attribute::Italic};
    mcurses::Glyph_string gs2{"⁗ 7₱", mcurses::Attribute::Italic,
                              mcurses::Attribute::Bold,
                              mcurses::background(mcurses::Color::Red)};

    EXPECT_TRUE(gs1 == gs2);
}

TEST(GlyphStringTest, Str) {
    mcurses::Glyph_string gs1{"⁗ 7₱", mcurses::Attribute::Bold,
                              mcurses::background(mcurses::Color::Red),
                              mcurses::Attribute::Italic};

    mcurses::Glyph_string gs2(std::string{"⎉ab_⍇⍈ ⍼"});

    mcurses::Glyph_string gs3{"", mcurses::Attribute::Bold,
                              mcurses::background(mcurses::Color::Yellow)};

    EXPECT_EQ(gs1.str(), "⁗ 7₱");
    EXPECT_EQ(gs2.str(), "⎉ab_⍇⍈ ⍼");
    EXPECT_EQ(gs3.str(), "");

    EXPECT_EQ(6, gs2.str().find("⍇"));

    EXPECT_EQ(4, gs2.str_u32().find(U"⍇"));
}

TEST(GlyphStringTest, OperatorPlus) {
    mcurses::Glyph_string gs1{"⁗ 7₱", mcurses::Attribute::Bold,
                              mcurses::background(mcurses::Color::Red),
                              mcurses::Attribute::Italic};

    mcurses::Glyph_string gs2("⎉ab_⍇⍈ ⍼", mcurses::Attribute::Italic);

    mcurses::Glyph_string gs = gs1 + gs2;

    EXPECT_EQ(gs, (mcurses::Glyph_string{{gs1[0], gs1[1], gs1[2], gs1[3],
                                          gs2[0], gs2[1], gs2[2], gs2[3],
                                          gs2[4], gs2[5], gs2[6], gs2[7]}}));
    EXPECT_EQ(gs[6], (mcurses::Glyph("b", mcurses::Attribute::Italic)));
}
