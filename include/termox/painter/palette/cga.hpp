#ifndef TERMOX_PAINTER_PALETTE_CGA_HPP
#define TERMOX_PAINTER_PALETTE_CGA_HPP
#include <termox/painter/color.hpp>

namespace ox::cga {

constexpr auto Black       = Color::Background;
constexpr auto Red         = Color{1};
constexpr auto Green       = Color{2};
constexpr auto Brown       = Color{3};
constexpr auto Blue        = Color{4};
constexpr auto Magenta     = Color{5};
constexpr auto Cyan        = Color{6};
constexpr auto Light_gray  = Color{7};
constexpr auto Gray        = Color{8};
constexpr auto Light_red   = Color{9};
constexpr auto Light_green = Color{10};
constexpr auto Yellow      = Color{11};
constexpr auto Teal        = Color{12};
constexpr auto Pink        = Color{13};
constexpr auto Light_blue  = Color{14};
constexpr auto White       = Color::Foreground;

/// Color Graphics Adaptor Palette
/** https://lospec.com/palette-list/color-graphics-adapter */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,       ANSI{16}, 0x000000},
    {Red,         ANSI{17}, 0xaa0000},
    {Green,       ANSI{18}, 0x00aa00},
    {Brown,       ANSI{19}, 0xaa5500},
    {Blue,        ANSI{20}, 0x0000aa},
    {Magenta,     ANSI{21}, 0xaa00aa},
    {Cyan,        ANSI{22}, 0x55ffff},
    {Light_gray,  ANSI{23}, 0xaaaaaa},
    {Gray,        ANSI{24}, 0x555555},
    {Light_red,   ANSI{25}, 0xff5555},
    {Light_green, ANSI{26}, 0x55ff55},
    {Yellow,      ANSI{27}, 0xffff55},
    {Teal,        ANSI{28}, 0x00aaaa},
    {Pink,        ANSI{29}, 0xff55ff},
    {Light_blue,  ANSI{30}, 0x5555ff},
    {White,       ANSI{31}, 0xffffff},
    /* clang-format on */
};

}  // namespace ox::cga
#endif  // TERMOX_PAINTER_PALETTE_CGA_HPP
