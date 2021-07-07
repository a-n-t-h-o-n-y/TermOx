#ifndef TERMOX_PAINTER_PALETTE_SECAM_HPP
#define TERMOX_PAINTER_PALETTE_SECAM_HPP
#include <termox/painter/color.hpp>

namespace ox::secam {

inline constexpr auto Black  = Color::Background;
inline constexpr auto Red    = Color{1};
inline constexpr auto Green  = Color{2};
inline constexpr auto Yellow = Color{3};
inline constexpr auto Blue   = Color{4};
inline constexpr auto Violet = Color{5};
inline constexpr auto Cyan   = Color{6};
inline constexpr auto White  = Color::Foreground;

/// SECAM Color Television 8 Color Palette
/** https://lospec.com/palette-list/secam */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,  RGB{0x000000}},
    {Red,    RGB{0xf03c79}},
    {Green,  RGB{0x7fff00}},
    {Yellow, RGB{0xffff3f}},
    {Blue,   RGB{0x2121ff}},
    {Violet, RGB{0xff50ff}},
    {Cyan,   RGB{0x7fffff}},
    {White,  RGB{0xffffff}},
    /* clang-format on */
};

}  // namespace ox::secam
#endif  // TERMOX_PAINTER_PALETTE_SECAM_HPP
