#ifndef TERMOX_PAINTER_PALETTE_DAWN_BRINGER16_HPP
#define TERMOX_PAINTER_PALETTE_DAWN_BRINGER16_HPP
#include <termox/painter/color.hpp>

namespace ox::dawn_bringer16 {

constexpr auto Black       = Color::Background;
constexpr auto Dark_red    = Color{1};
constexpr auto Green       = Color{2};
constexpr auto Brown       = Color{3};
constexpr auto Dark_blue   = Color{4};
constexpr auto Violet      = Color{5};
constexpr auto Light_blue  = Color{6};
constexpr auto Light_gray  = Color{7};
constexpr auto Dark_gray   = Color{8};
constexpr auto Red         = Color{9};
constexpr auto Light_green = Color{10};
constexpr auto Yellow      = Color{11};
constexpr auto Blue        = Color{12};
constexpr auto Orange      = Color{13};
constexpr auto Gray        = Color{14};
constexpr auto White       = Color::Foreground;

/// Dawn Bringer 16 Color Palette
/** https://lospec.com/palette-list/dawnbringer-16 */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,       ANSI{16}, 0x140c1c},
    {Dark_red,    ANSI{17}, 0x442434},
    {Green,       ANSI{18}, 0x346524},
    {Brown,       ANSI{19}, 0x854c30},
    {Dark_blue,   ANSI{20}, 0x30346d},
    {Violet,      ANSI{21}, 0xd2aa99},
    {Light_blue,  ANSI{22}, 0x6dc2ca},
    {Light_gray,  ANSI{23}, 0x8595a1},
    {Dark_gray,   ANSI{24}, 0x4e4a4e},
    {Red,         ANSI{25}, 0xd04648},
    {Light_green, ANSI{26}, 0x6daa2c},
    {Yellow,      ANSI{27}, 0xdad45e},
    {Blue,        ANSI{28}, 0x597dce},
    {Orange,      ANSI{29}, 0xd27d2c},
    {Gray,        ANSI{30}, 0x757161},
    {White,       ANSI{31}, 0xdeeed6},
    /* clang-format on */
};

}  // namespace ox::dawn_bringer16
#endif  // TERMOX_PAINTER_PALETTE_DAWN_BRINGER16_HPP
