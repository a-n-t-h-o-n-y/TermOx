#ifndef TERMOX_PAINTER_PALETTE_SECAM_HPP
#define TERMOX_PAINTER_PALETTE_SECAM_HPP
#include <termox/painter/color.hpp>

namespace ox::secam {

constexpr auto Black  = Color::Background;
constexpr auto Red    = Color{1};
constexpr auto Green  = Color{2};
constexpr auto Yellow = Color{3};
constexpr auto Blue   = Color{4};
constexpr auto Violet = Color{5};
constexpr auto Cyan   = Color{6};
constexpr auto White  = Color::Foreground;

/// SECAM Color Television 8 Color Palette
/** https://lospec.com/palette-list/secam */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,  ANSI{16}, 0x000000},
    {Red,    ANSI{17}, 0xf03c79},
    {Green,  ANSI{18}, 0x7fff00},
    {Yellow, ANSI{19}, 0xffff3f},
    {Blue,   ANSI{20}, 0x2121ff},
    {Violet, ANSI{21}, 0xff50ff},
    {Cyan,   ANSI{22}, 0x7fffff},
    {White,  ANSI{23}, 0xffffff},
    /* clang-format on */
};

}  // namespace ox::secam
#endif  // TERMOX_PAINTER_PALETTE_SECAM_HPP
