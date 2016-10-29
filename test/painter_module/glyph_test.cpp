#include <mcurses/painter_module/glyph.hpp>
#include <gtest/gtest.h>

#include <string>

std::string foo(const mcurses::Glyph& g) {
	return g.str();
}

TEST(GlyphTest, Constructor) {
	EXPECT_NO_THROW(mcurses::Glyph("H"));
	EXPECT_NO_THROW(mcurses::Glyph(5));
	EXPECT_NO_THROW(mcurses::Glyph('j'));
	const char cp[] = "₹";
	EXPECT_NO_THROW(mcurses::Glyph{cp});
	EXPECT_NO_THROW(mcurses::Glyph("ↈ"));
	EXPECT_THROW(mcurses::Glyph("ↈↈ"), mcurses::Glyph_error);
	EXPECT_THROW(mcurses::Glyph("ↈ "), mcurses::Glyph_error);

	const char long_cp[] = "hello";
	std::string s = "hello";

	EXPECT_THROW(mcurses::Glyph{54}, mcurses::Glyph_error);
	EXPECT_THROW(mcurses::Glyph{long_cp}, mcurses::Glyph_error);
	EXPECT_THROW(mcurses::Glyph{s}, mcurses::Glyph_error);
	EXPECT_THROW(mcurses::Glyph("World!"), mcurses::Glyph_error);
}

TEST(GlyphTest, Str) {
	mcurses::Glyph glf_1("H");
	mcurses::Glyph glf_2("ↈ");
	mcurses::Glyph glf_3(6);

	EXPECT_EQ("H", glf_1.str());
	EXPECT_EQ("ↈ", glf_2.str());
	EXPECT_EQ("6", glf_3.str());
}