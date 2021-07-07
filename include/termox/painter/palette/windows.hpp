#ifndef TERMOX_PAINTER_PALETTE_WINDOWS_HPP
#define TERMOX_PAINTER_PALETTE_WINDOWS_HPP
#include <termox/painter/color.hpp>

namespace ox::windows {

inline constexpr auto Black    = Color::Background;
inline constexpr auto Maroon   = Color{1};
inline constexpr auto Green    = Color{2};
inline constexpr auto Olive    = Color{3};
inline constexpr auto Navy     = Color{4};
inline constexpr auto Violet   = Color{5};
inline constexpr auto Cyan     = Color{6};
inline constexpr auto Silver   = Color{7};
inline constexpr auto Gray     = Color{8};
inline constexpr auto Red      = Color{9};
inline constexpr auto Lime     = Color{10};
inline constexpr auto Yellow   = Color{11};
inline constexpr auto Blue     = Color{12};
inline constexpr auto Eggplant = Color{13};
inline constexpr auto Teal     = Color{14};
inline constexpr auto White    = Color::Foreground;

/// Early Microsoft Windows 16 Color Palette
/** https://lospec.com/palette-list/microsoft-windows */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,    RGB{0x000000}},
    {Maroon,   RGB{0x7e0000}},
    {Green,    RGB{0x047e00}},
    {Olive,    RGB{0x7e7e00}},
    {Navy,     RGB{0x00007e}},
    {Violet,   RGB{0xfe00ff}},
    {Cyan,     RGB{0x06ffff}},
    {Silver,   RGB{0xbebebe}},
    {Gray,     RGB{0x7e7e7e}},
    {Red,      RGB{0xfe0000}},
    {Lime,     RGB{0x06ff04}},
    {Yellow,   RGB{0xffff04}},
    {Blue,     RGB{0x0000ff}},
    {Eggplant, RGB{0x7e007e}},
    {Teal,     RGB{0x047e7e}},
    {White,    RGB{0xffffff}},
    /* clang-format on */
};

}  // namespace ox::windows
#endif  // TERMOX_PAINTER_PALETTE_WINDOWS_HPP
