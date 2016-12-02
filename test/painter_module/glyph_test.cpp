#include <mcurses/painter_module/glyph.hpp>
#include <mcurses/painter_module/attribute.hpp>
#include <mcurses/painter_module/color.hpp>
#include <mcurses/painter_module/brush.hpp>
#include <gtest/gtest.h>

#include <string>

std::string foo(const mcurses::Glyph& g) {
	return g.str();
}

TEST(GlyphTest, StringConstructor) {
	EXPECT_NO_THROW(mcurses::Glyph("H"));
	EXPECT_NO_THROW(mcurses::Glyph("5"));
	EXPECT_NO_THROW(mcurses::Glyph('j'));
	const char cp[] = "₹";
	EXPECT_NO_THROW(mcurses::Glyph{cp});
	EXPECT_NO_THROW(mcurses::Glyph("ↈ"));
	EXPECT_THROW(mcurses::Glyph("ↈↈ"), mcurses::Glyph_error);
	EXPECT_THROW(mcurses::Glyph("ↈ "), mcurses::Glyph_error);

	const char long_cp[] = "hello";
	std::string s = "hello";

	EXPECT_THROW(mcurses::Glyph{"54"}, mcurses::Glyph_error);
	EXPECT_THROW(mcurses::Glyph{long_cp}, mcurses::Glyph_error);
	EXPECT_THROW(mcurses::Glyph{s}, mcurses::Glyph_error);
	EXPECT_THROW(mcurses::Glyph("World!"), mcurses::Glyph_error);
}

TEST(GlyphTest, StringAndAttributesConstructor) {
	EXPECT_NO_THROW(mcurses::Glyph("#", mcurses::Attribute::Bold));
	EXPECT_NO_THROW(mcurses::Glyph("1", mcurses::background(mcurses::Color::Blue), mcurses::Attribute::Underline, mcurses::Attribute::Bold, mcurses::Attribute::Underline));
	EXPECT_NO_THROW(mcurses::Glyph("ޒ", mcurses::Attribute::Standout));

	const char long_cp[] = "hello";
	std::string s = "hello";

	EXPECT_THROW((mcurses::Glyph{"54", mcurses::Attribute::Bold, mcurses::background(mcurses::Color::Light_blue)}), mcurses::Glyph_error);
	EXPECT_THROW((mcurses::Glyph{long_cp, mcurses::Attribute::Dim, mcurses::foreground(mcurses::Color::Black), mcurses::foreground(mcurses::Color::Dark_red)}), mcurses::Glyph_error);
	EXPECT_THROW(mcurses::Glyph{s}, mcurses::Glyph_error);
	EXPECT_THROW(mcurses::Glyph("World!", mcurses::Attribute::Italic, mcurses::foreground(mcurses::Color::Red)), mcurses::Glyph_error);
}

TEST(GlyphTest, Symbol) {
	mcurses::Glyph glf_1("H");
	mcurses::Glyph glf_2("ↈ");
	mcurses::Glyph glf_3("6");

	EXPECT_EQ("H", glf_1.str());
	EXPECT_EQ("ↈ", glf_2.str());
	EXPECT_EQ("6", glf_3.str());
}

TEST(GlyphTest, SetSymbol) {
	mcurses::Glyph g1 = "ੴ";
	mcurses::Glyph g2 = {"ആ", mcurses::Attribute::Italic, mcurses::background(mcurses::Color::Green)};
	mcurses::Glyph g3{"g", mcurses::foreground(mcurses::Color::Red), mcurses::Attribute::Dim};

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

TEST(GlyphTest, SetBrush) {
	mcurses::Glyph g1{"ៀ"};
	mcurses::Glyph g2("ᯣ", mcurses::Attribute::Italic, mcurses::background(mcurses::Color::White));

	EXPECT_EQ(mcurses::Brush{}, g1.brush());
	EXPECT_EQ((mcurses::Brush{mcurses::Attribute::Italic, mcurses::background(mcurses::Color::White)}), g2.brush());

	g2.brush().set_foreground(mcurses::Color::Yellow);
	g2.brush().add_attributes(mcurses::Attribute::Standout);

	EXPECT_EQ((mcurses::Brush{mcurses::Attribute::Standout, mcurses::Attribute::Italic, mcurses::background(mcurses::Color::White), mcurses::foreground(mcurses::Color::Yellow)}), g2.brush());
}