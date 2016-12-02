#include <mcurses/painter_module/glyph_matrix.hpp>
#include <gtest/gtest.h>

#include <stdexcept>

TEST(GlyphMatrixTest, Default) {
	using namespace mcurses;
	Glyph_matrix gm{3, 3};
	EXPECT_NO_THROW(gm.at(1,1));
	EXPECT_NO_THROW(gm.at(0,0));
	EXPECT_NO_THROW(gm.at(2,2));
	EXPECT_NO_THROW(gm.at(0,1));
	EXPECT_NO_THROW(gm.at(2,0));
	EXPECT_NO_THROW(gm.at(0,2));

	EXPECT_THROW(gm.at(3,2), std::out_of_range);
	EXPECT_THROW(gm.at(3,3), std::out_of_range);
	EXPECT_THROW(gm.at(0,5), std::out_of_range);
	EXPECT_THROW(gm.at(300,200), std::out_of_range);

	Glyph_matrix empty;
	EXPECT_THROW(empty.at(3,2), std::out_of_range);
	EXPECT_THROW(empty.at(0,0), std::out_of_range);
}