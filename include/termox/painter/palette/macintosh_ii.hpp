#ifndef TERMOX_PAINTER_PALETTE_MACINTOSH_II_HPP
#define TERMOX_PAINTER_PALETTE_MACINTOSH_II_HPP
#include <termox/painter/color.hpp>

namespace ox::macintosh_ii {

inline constexpr auto Black       = Color::Background;
inline constexpr auto Red         = Color{1};
inline constexpr auto Green       = Color{2};
inline constexpr auto Brown       = Color{3};
inline constexpr auto Indigo      = Color{4};
inline constexpr auto Magenta     = Color{5};
inline constexpr auto Light_blue  = Color{6};
inline constexpr auto Silver      = Color{7};
inline constexpr auto Dark_gray   = Color{8};
inline constexpr auto Light_brown = Color{9};
inline constexpr auto Light_green = Color{10};
inline constexpr auto Yellow      = Color{11};
inline constexpr auto Blue        = Color{12};
inline constexpr auto Orange      = Color{13};
inline constexpr auto Gray        = Color{14};
inline constexpr auto White       = Color::Foreground;

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
