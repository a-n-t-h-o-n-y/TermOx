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
    {Black,       ANSI{16}, 0x000000},
    {Red,         ANSI{17}, 0xdc0000},
    {Green,       ANSI{18}, 0x006500},
    {Brown,       ANSI{19}, 0x653600},
    {Indigo,      ANSI{20}, 0x360097},
    {Magenta,     ANSI{21}, 0xff0097},
    {Light_blue,  ANSI{22}, 0x0097ff},
    {Silver,      ANSI{23}, 0xb9b9b9},
    {Dark_gray,   ANSI{24}, 0x454545},
    {Light_brown, ANSI{25}, 0x976536},
    {Light_green, ANSI{26}, 0x00a800},
    {Yellow,      ANSI{27}, 0xffff00},
    {Blue,        ANSI{28}, 0x0000ca},
    {Orange,      ANSI{29}, 0xff6500},
    {Gray,        ANSI{30}, 0x868686},
    {White,       ANSI{31}, 0xffffff},
    /* clang-format on */
};

}  // namespace ox::macintosh_ii
#endif  // TERMOX_PAINTER_PALETTE_MACINTOSH_II_HPP
