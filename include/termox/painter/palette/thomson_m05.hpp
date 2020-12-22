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
    {Black,      ANSI{16}, 0x000000},
    {Light_red,  ANSI{17}, 0xdd7777},
    {Mint,       ANSI{18}, 0x77dd77},
    {Brown,      ANSI{19}, 0xdddd77},
    {Light_blue, ANSI{20}, 0x7777dd},
    {Violet,     ANSI{21}, 0xdd77ee},
    {Light_cyan, ANSI{22}, 0xbbffff},
    {Silver,     ANSI{23}, 0xbbbbbb},
    {Orange,     ANSI{24}, 0xeebb00},
    {Red,        ANSI{25}, 0xff0000},
    {Lime,       ANSI{26}, 0x00ff00},
    {Yellow,     ANSI{27}, 0xffff00},
    {Blue,       ANSI{28}, 0x0000ff},
    {Magenta,    ANSI{29}, 0xff00ff},
    {Cyan,       ANSI{30}, 0x00ffff},
    {White,      ANSI{31}, 0xffffff},
    /* clang-format on */
};

}  // namespace ox::thomson_m05
#endif  // TERMOX_PAINTER_PALETTE_THOMSON_M05_HPP
