#ifndef TERMOX_PAINTER_PALETTE_MACINTOSH_II_HPP
#define TERMOX_PAINTER_PALETTE_MACINTOSH_II_HPP
#include <termox/painter/color.hpp>

namespace ox::macintosh_ii {

constexpr auto Black       = Color::Background;
constexpr auto Red         = Color{1};
constexpr auto Green       = Color{2};
constexpr auto Brown       = Color{3};
constexpr auto Indigo      = Color{4};
constexpr auto Magenta     = Color{5};
constexpr auto Light_blue  = Color{6};
constexpr auto Silver      = Color{7};
constexpr auto Dark_gray   = Color{8};
constexpr auto Light_brown = Color{9};
constexpr auto Light_green = Color{10};
constexpr auto Yellow      = Color{11};
constexpr auto Blue        = Color{12};
constexpr auto Orange      = Color{13};
constexpr auto Gray        = Color{14};
constexpr auto White       = Color::Foreground;

/// Macintosh II 16 Color Palette
/** https://lospec.com/palette-list/macintosh-ii */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,       RGB{0x000000}},
    {Red,         RGB{0xdc0000}},
    {Green,       RGB{0x006500}},
    {Brown,       RGB{0x653600}},
    {Indigo,      RGB{0x360097}},
    {Magenta,     RGB{0xff0097}},
    {Light_blue,  RGB{0x0097ff}},
    {Silver,      RGB{0xb9b9b9}},
    {Dark_gray,   RGB{0x454545}},
    {Light_brown, RGB{0x976536}},
    {Light_green, RGB{0x00a800}},
    {Yellow,      RGB{0xffff00}},
    {Blue,        RGB{0x0000ca}},
    {Orange,      RGB{0xff6500}},
    {Gray,        RGB{0x868686}},
    {White,       RGB{0xffffff}},
    /* clang-format on */
};

}  // namespace ox::macintosh_ii
#endif  // TERMOX_PAINTER_PALETTE_MACINTOSH_II_HPP
