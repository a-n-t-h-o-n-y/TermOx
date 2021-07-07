#ifndef TERMOX_PAINTER_PALETTE_GAMEBOY_HPP
#define TERMOX_PAINTER_PALETTE_GAMEBOY_HPP
#include <termox/painter/color.hpp>

namespace ox::gameboy {

inline constexpr auto Green_1 = Color::Background;
inline constexpr auto Green_2 = Color{2};
inline constexpr auto Green_3 = Color{3};
inline constexpr auto Green_4 = Color::Foreground;

/// Original Gameboy Greenscale 4 Color Palette
inline auto const palette = Palette{
    /* clang-format off */
    {Green_1, RGB{0x0f380f}},
    {Green_2, RGB{0x306230}},
    {Green_3, RGB{0x8bac0f}},
    {Green_4, RGB{0x9bbc0f}},
    /* clang-format on */
};

}  // namespace ox::gameboy
#endif  // TERMOX_PAINTER_PALETTE_GAMEBOY_HPP
