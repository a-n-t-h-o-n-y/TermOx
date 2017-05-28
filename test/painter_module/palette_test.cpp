#include <painter_module/color.hpp>
#include <painter_module/palette.hpp>
#include <system_module/system.hpp>

#include <gtest/gtest.h>

using cppurses::Color;
using cppurses::DawnBringer_palette;
using cppurses::Standard_palette;
using cppurses::System;

TEST(PaletteTest, Everything) {
    System system;
    EXPECT_EQ(255, Standard_palette().red_value(Color::Red));
    EXPECT_EQ(0, Standard_palette().green_value(Color::Red));
    EXPECT_EQ(0, Standard_palette().blue_value(Color::Red));

    EXPECT_EQ(52, DawnBringer_palette().red_value(Color::Green));
    EXPECT_EQ(101, DawnBringer_palette().green_value(Color::Green));
    EXPECT_EQ(36, DawnBringer_palette().blue_value(Color::Green));

    EXPECT_EQ(208, System::palette()->red_value(Color::Red));
    System::set_palette(std::make_unique<Standard_palette>());
    EXPECT_EQ(255, System::palette()->red_value(Color::Red));
}
