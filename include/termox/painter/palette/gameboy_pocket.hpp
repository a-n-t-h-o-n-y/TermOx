#ifndef TERMOX_PAINTER_PALETTE_GAMEBOY_POCKET_HPP
#define TERMOX_PAINTER_PALETTE_GAMEBOY_POCKET_HPP
#include <termox/painter/color.hpp>

namespace ox::gameboy_pocket {

constexpr auto Black      = Color::Background;
constexpr auto Dark_gray  = Color{8};
constexpr auto Light_gray = Color{7};
constexpr auto White      = Color::Foreground;

/// Gameboy Pocket Grayscale Color Palette
inline auto const palette = Palette{
    /* clang-format off */
    {Black,      RGB{0x000000}},
    {Dark_gray,  RGB{0x545454}},
    {Light_gray, RGB{0xa9a9a9}},
    {White,      RGB{0xffffff}},
    /* clang-format on */
};

}  // namespace ox::gameboy_pocket
#endif  // TERMOX_PAINTER_PALETTE_GAMEBOY_POCKET_HPP
