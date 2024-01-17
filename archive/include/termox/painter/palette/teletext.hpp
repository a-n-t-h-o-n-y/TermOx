#ifndef TERMOX_PAINTER_PALETTE_TELETEXT_HPP
#define TERMOX_PAINTER_PALETTE_TELETEXT_HPP
#include <termox/painter/color.hpp>

namespace ox::teletext {

inline constexpr auto Black   = Color::Background;
inline constexpr auto Blue    = Color{4};
inline constexpr auto Red     = Color{1};
inline constexpr auto Magenta = Color{5};
inline constexpr auto Lime    = Color{2};
inline constexpr auto Cyan    = Color{6};
inline constexpr auto Yellow  = Color{3};
inline constexpr auto White   = Color::Foreground;

/// World System Teletext Level 1 Color Palette
inline auto const palette = Palette{
    /* clang-format off */
    {Black,   RGB{0x000000}},
    {Blue,    RGB{0x0000ff}},
    {Red,     RGB{0xff0000}},
    {Magenta, RGB{0xff00ff}},
    {Lime,    RGB{0x00ff00}},
    {Cyan,    RGB{0x00ffff}},
    {Yellow,  RGB{0xffff00}},
    {White,   RGB{0xffffff}},
    /* clang-format on */
};

}  // namespace ox::teletext
#endif  // TERMOX_PAINTER_PALETTE_TELETEXT_HPP
