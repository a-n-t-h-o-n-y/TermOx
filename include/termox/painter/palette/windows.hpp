#ifndef TERMOX_PAINTER_PALETTE_WINDOWS_HPP
#define TERMOX_PAINTER_PALETTE_WINDOWS_HPP
#include <termox/painter/color.hpp>

namespace ox::windows {

constexpr auto Black    = Color::Background;
constexpr auto Maroon   = Color{1};
constexpr auto Green    = Color{2};
constexpr auto Olive    = Color{3};
constexpr auto Navy     = Color{4};
constexpr auto Violet   = Color{5};
constexpr auto Cyan     = Color{6};
constexpr auto Silver   = Color{7};
constexpr auto Gray     = Color{8};
constexpr auto Red      = Color{9};
constexpr auto Lime     = Color{10};
constexpr auto Yellow   = Color{11};
constexpr auto Blue     = Color{12};
constexpr auto Eggplant = Color{13};
constexpr auto Teal     = Color{14};
constexpr auto White    = Color::Foreground;

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
