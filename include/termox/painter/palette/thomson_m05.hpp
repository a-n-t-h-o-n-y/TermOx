#ifndef TERMOX_PAINTER_PALETTE_THOMSON_M05_HPP
#define TERMOX_PAINTER_PALETTE_THOMSON_M05_HPP
#include <termox/painter/color.hpp>

namespace ox::thomson_m05 {

inline constexpr auto Black      = Color::Background;
inline constexpr auto Light_red  = Color{1};
inline constexpr auto Mint       = Color{2};
inline constexpr auto Brown      = Color{3};
inline constexpr auto Light_blue = Color{4};
inline constexpr auto Violet     = Color{5};
inline constexpr auto Light_cyan = Color{6};
inline constexpr auto Silver     = Color{7};
inline constexpr auto Orange     = Color{8};
inline constexpr auto Red        = Color{9};
inline constexpr auto Lime       = Color{10};
inline constexpr auto Yellow     = Color{11};
inline constexpr auto Blue       = Color{12};
inline constexpr auto Magenta    = Color{13};
inline constexpr auto Cyan       = Color{14};
inline constexpr auto White      = Color::Foreground;

/// Thomson M05 16 Color Palette
/** https://lospec.com/palette-list/thomson-m05 */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,      RGB{0x000000}},
    {Light_red,  RGB{0xdd7777}},
    {Mint,       RGB{0x77dd77}},
    {Brown,      RGB{0xdddd77}},
    {Light_blue, RGB{0x7777dd}},
    {Violet,     RGB{0xdd77ee}},
    {Light_cyan, RGB{0xbbffff}},
    {Silver,     RGB{0xbbbbbb}},
    {Orange,     RGB{0xeebb00}},
    {Red,        RGB{0xff0000}},
    {Lime,       RGB{0x00ff00}},
    {Yellow,     RGB{0xffff00}},
    {Blue,       RGB{0x0000ff}},
    {Magenta,    RGB{0xff00ff}},
    {Cyan,       RGB{0x00ffff}},
    {White,      RGB{0xffffff}},
    /* clang-format on */
};

}  // namespace ox::thomson_m05
#endif  // TERMOX_PAINTER_PALETTE_THOMSON_M05_HPP
