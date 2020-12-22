#include <painter/color.hpp>
#include <painter/glyph.hpp>
#include <painter/glyph_matrix.hpp>
#include <painter/trait.hpp>

#include <gtest/gtest.h>

#include <stdexcept>

using ox::bg;
using ox::Color;
using ox::foreground;
using ox::Glyph;
using ox::Glyph_matrix;
using ox::Trait;

TEST(GlyphMatrixTest, Default)
{
    const Glyph_matrix gm{3, 3};
    EXPECT_NO_THROW(gm.at(1, 1));
    EXPECT_NO_THROW(gm.at(0, 0));
    EXPECT_NO_THROW(gm.at(2, 2));
    EXPECT_NO_THROW(gm.at(0, 1));
    EXPECT_NO_THROW(gm.at(2, 0));
    EXPECT_NO_THROW(gm.at(0, 2));

    EXPECT_THROW(gm.at(3, 2), std::out_of_range);
    EXPECT_THROW(gm.at(3, 3), std::out_of_range);
    EXPECT_THROW(gm.at(0, 5), std::out_of_range);
    EXPECT_THROW(gm.at(300, 200), std::out_of_range);

    Glyph_matrix empty;
    EXPECT_THROW(empty.at(3, 2), std::out_of_range);
    EXPECT_THROW(empty.at(0, 0), std::out_of_range);

    Glyph_matrix gm1;
    EXPECT_EQ(0, gm1.width());
    EXPECT_EQ(0, gm1.height());

    gm1.resize(3, 2);

    EXPECT_EQ(3, gm1.width());
    EXPECT_EQ(2, gm1.height());

    const Glyph_matrix cgm(4, 5);
    ASSERT_EQ(4, cgm.width());
    ASSERT_EQ(5, cgm.height());

    Glyph_matrix gm2(5, 3);
    gm2.at(4, 2) = Glyph{"Ѯ", bg(Color::Orange), Trait::Bold};
    EXPECT_EQ("Ѯ", gm2.at(4, 2).str());
    EXPECT_EQ(" ", gm2.at(0, 0).str());
    EXPECT_THROW(gm2.at(5, 3), std::out_of_range);
    EXPECT_THROW(gm2.at(4, 3), std::out_of_range);
    EXPECT_THROW(gm2.at(5, 2), std::out_of_range);
}
