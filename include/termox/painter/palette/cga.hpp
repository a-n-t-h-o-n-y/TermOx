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
    {Black,       RGB{0x000000}},
    {Red,         RGB{0xaa0000}},
    {Green,       RGB{0x00aa00}},
    {Brown,       RGB{0xaa5500}},
    {Blue,        RGB{0x0000aa}},
    {Magenta,     RGB{0xaa00aa}},
    {Cyan,        RGB{0x55ffff}},
    {Light_gray,  RGB{0xaaaaaa}},
    {Gray,        RGB{0x555555}},
    {Light_red,   RGB{0xff5555}},
    {Light_green, RGB{0x55ff55}},
    {Yellow,      RGB{0xffff55}},
    {Teal,        RGB{0x00aaaa}},
    {Pink,        RGB{0xff55ff}},
    {Light_blue,  RGB{0x5555ff}},
    {White,       RGB{0xffffff}},
    /* clang-format on */
};

}  // namespace ox::cga
#endif  // TERMOX_PAINTER_PALETTE_CGA_HPP
