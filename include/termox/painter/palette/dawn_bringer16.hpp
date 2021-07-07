#ifndef TERMOX_PAINTER_PALETTE_DAWN_BRINGER16_HPP
#define TERMOX_PAINTER_PALETTE_DAWN_BRINGER16_HPP
#include <termox/painter/color.hpp>

namespace ox::dawn_bringer16 {

inline constexpr auto Black       = Color::Background;
inline constexpr auto Dark_red    = Color{1};
inline constexpr auto Green       = Color{2};
inline constexpr auto Brown       = Color{3};
inline constexpr auto Dark_blue   = Color{4};
inline constexpr auto Violet      = Color{5};
inline constexpr auto Light_blue  = Color{6};
inline constexpr auto Light_gray  = Color{7};
inline constexpr auto Dark_gray   = Color{8};
inline constexpr auto Red         = Color{9};
inline constexpr auto Light_green = Color{10};
inline constexpr auto Yellow      = Color{11};
inline constexpr auto Blue        = Color{12};
inline constexpr auto Orange      = Color{13};
inline constexpr auto Gray        = Color{14};
inline constexpr auto White       = Color::Foreground;

/// Dawn Bringer 16 Color Palette
/** https://lospec.com/palette-list/dawnbringer-16 */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,       RGB{0x140c1c}},
    {Dark_red,    RGB{0x442434}},
    {Green,       RGB{0x346524}},
    {Brown,       RGB{0x854c30}},
    {Dark_blue,   RGB{0x30346d}},
    {Violet,      RGB{0xd2aa99}},
    {Light_blue,  RGB{0x6dc2ca}},
    {Light_gray,  RGB{0x8595a1}},
    {Dark_gray,   RGB{0x4e4a4e}},
    {Red,         RGB{0xd04648}},
    {Light_green, RGB{0x6daa2c}},
    {Yellow,      RGB{0xdad45e}},
    {Blue,        RGB{0x597dce}},
    {Orange,      RGB{0xd27d2c}},
    {Gray,        RGB{0x757161}},
    {White,       RGB{0xdeeed6}},
    /* clang-format on */
};

}  // namespace ox::dawn_bringer16
#endif  // TERMOX_PAINTER_PALETTE_DAWN_BRINGER16_HPP
