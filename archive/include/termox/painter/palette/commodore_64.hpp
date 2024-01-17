#ifndef TERMOX_PAINTER_PALETTE_COMMODORE_64_HPP
#define TERMOX_PAINTER_PALETTE_COMMODORE_64_HPP
#include <termox/painter/color.hpp>

namespace ox::commodore_64 {

inline constexpr auto Black       = Color::Background;
inline constexpr auto Dark_red    = Color{1};
inline constexpr auto Green       = Color{2};
inline constexpr auto Brown       = Color{3};
inline constexpr auto Blue        = Color{4};
inline constexpr auto Violet      = Color{5};
inline constexpr auto Cyan        = Color{6};
inline constexpr auto Light_gray  = Color{7};
inline constexpr auto Gray        = Color{8};
inline constexpr auto Red         = Color{9};
inline constexpr auto Light_green = Color{10};
inline constexpr auto Yellow      = Color{11};
inline constexpr auto Light_blue  = Color{12};
inline constexpr auto Orange      = Color{13};
inline constexpr auto Silver      = Color{14};
inline constexpr auto White       = Color::Foreground;

/// Commodore 64 16 Color Palette
/** https://lospec.com/palette-list/commodore-64 */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,       RGB{0x000000}},
    {Dark_red,    RGB{0x813338}},
    {Green,       RGB{0x56ac4d}},
    {Brown,       RGB{0x553800}},
    {Blue,        RGB{0x2e2c9b}},
    {Violet,      RGB{0x8e3c97}},
    {Cyan,        RGB{0x75cec8}},
    {Light_gray,  RGB{0x7b7b7b}},
    {Gray,        RGB{0x4a4a4a}},
    {Red,         RGB{0xc46c71}},
    {Light_green, RGB{0xa9ff9f}},
    {Yellow,      RGB{0xedf171}},
    {Light_blue,  RGB{0x706deb}},
    {Orange,      RGB{0x8e5029}},
    {Silver,      RGB{0xb2b2b2}},
    {White,       RGB{0xffffff}},
    /* clang-format on */
};

}  // namespace ox::commodore_64
#endif  // TERMOX_PAINTER_PALETTE_COMMODORE_64_HPP
