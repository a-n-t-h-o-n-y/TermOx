#ifndef TERMOX_PAINTER_PALETTE_GAMEBOY_HPP
#define TERMOX_PAINTER_PALETTE_GAMEBOY_HPP
#include <termox/painter/color.hpp>

namespace ox::gameboy {

constexpr auto Green_1 = Color::Background;
constexpr auto Green_2 = Color{2};
constexpr auto Green_3 = Color{3};
constexpr auto Green_4 = Color::Foreground;

/// Original Gameboy Greenscale 4 Color Palette
inline auto const palette = Palette{
    /* clang-format off */
    {Green_1, ANSI{16}, 0x0f380f},
    {Green_2, ANSI{17}, 0x306230},
    {Green_3, ANSI{18}, 0x8bac0f},
    {Green_4, ANSI{19}, 0x9bbc0f},
    /* clang-format on */
};

}  // namespace ox::gameboy
#endif  // TERMOX_PAINTER_PALETTE_GAMEBOY_HPP
