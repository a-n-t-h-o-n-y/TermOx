#include <mcurses/painter_module/palette.hpp>
#include <mcurses/painter_module/color.hpp>
#include <mcurses/system_module/system.hpp>
#include <gtest/gtest.h>

TEST(PaletteTest, Everything){
	EXPECT_EQ(255, mcurses::Standard_palette().red_value(mcurses::Color::Red));
	EXPECT_EQ(0, mcurses::Standard_palette().green_value(mcurses::Color::Red));
	EXPECT_EQ(0, mcurses::Standard_palette().blue_value(mcurses::Color::Red));

	EXPECT_EQ(52, mcurses::DawnBringer_palette().red_value(mcurses::Color::Green));
	EXPECT_EQ(101, mcurses::DawnBringer_palette().green_value(mcurses::Color::Green));
	EXPECT_EQ(36, mcurses::DawnBringer_palette().blue_value(mcurses::Color::Green));

	EXPECT_EQ(208, mcurses::System::palette()->red_value(mcurses::Color::Red));
	mcurses::System::set_palette(std::make_unique<mcurses::Standard_palette>());
	EXPECT_EQ(255, mcurses::System::palette()->red_value(mcurses::Color::Red));
}