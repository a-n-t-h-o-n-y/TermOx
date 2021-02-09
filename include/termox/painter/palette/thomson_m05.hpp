#ifndef TERMOX_PAINTER_PALETTE_THOMSON_M05_HPP
#define TERMOX_PAINTER_PALETTE_THOMSON_M05_HPP
#include <termox/painter/color.hpp>

namespace ox::thomson_m05 {

constexpr auto Black      = Color::Background;
constexpr auto Light_red  = Color{1};
constexpr auto Mint       = Color{2};
constexpr auto Brown      = Color{3};
constexpr auto Light_blue = Color{4};
constexpr auto Violet     = Color{5};
constexpr auto Light_cyan = Color{6};
constexpr auto Silver     = Color{7};
constexpr auto Orange     = Color{8};
constexpr auto Red        = Color{9};
constexpr auto Lime       = Color{10};
constexpr auto Yellow     = Color{11};
constexpr auto Blue       = Color{12};
constexpr auto Magenta    = Color{13};
constexpr auto Cyan       = Color{14};
constexpr auto White      = Color::Foreground;

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
