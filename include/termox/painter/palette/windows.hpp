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
    {Black,    ANSI{16}, 0x000000},
    {Maroon,   ANSI{17}, 0x7e0000},
    {Green,    ANSI{18}, 0x047e00},
    {Olive,    ANSI{19}, 0x7e7e00},
    {Navy,     ANSI{20}, 0x00007e},
    {Violet,   ANSI{21}, 0xfe00ff},
    {Cyan,     ANSI{22}, 0x06ffff},
    {Silver,   ANSI{23}, 0xbebebe},
    {Gray,     ANSI{24}, 0x7e7e7e},
    {Red,      ANSI{25}, 0xfe0000},
    {Lime,     ANSI{26}, 0x06ff04},
    {Yellow,   ANSI{27}, 0xffff04},
    {Blue,     ANSI{28}, 0x0000ff},
    {Eggplant, ANSI{29}, 0x7e007e},
    {Teal,     ANSI{30}, 0x047e7e},
    {White,    ANSI{31}, 0xffffff},
    /* clang-format on */
};

}  // namespace ox::windows
#endif  // TERMOX_PAINTER_PALETTE_WINDOWS_HPP
