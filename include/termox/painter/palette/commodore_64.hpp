#ifndef TERMOX_PAINTER_PALETTE_COMMODORE_64_HPP
#define TERMOX_PAINTER_PALETTE_COMMODORE_64_HPP
#include <termox/painter/color.hpp>

namespace ox::commodore_64 {

constexpr auto Black       = Color::Background;
constexpr auto Dark_red    = Color{1};
constexpr auto Green       = Color{2};
constexpr auto Brown       = Color{3};
constexpr auto Blue        = Color{4};
constexpr auto Violet      = Color{5};
constexpr auto Cyan        = Color{6};
constexpr auto Light_gray  = Color{7};
constexpr auto Gray        = Color{8};
constexpr auto Red         = Color{9};
constexpr auto Light_green = Color{10};
constexpr auto Yellow      = Color{11};
constexpr auto Light_blue  = Color{12};
constexpr auto Orange      = Color{13};
constexpr auto Silver      = Color{14};
constexpr auto White       = Color::Foreground;

/// Commodore 64 16 Color Palette
/** https://lospec.com/palette-list/commodore-64 */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,       ANSI{16}, 0x000000},
    {Dark_red,    ANSI{17}, 0x813338},
    {Green,       ANSI{18}, 0x56ac4d},
    {Brown,       ANSI{19}, 0x553800},
    {Blue,        ANSI{20}, 0x2e2c9b},
    {Violet,      ANSI{21}, 0x8e3c97},
    {Cyan,        ANSI{22}, 0x75cec8},
    {Light_gray,  ANSI{23}, 0x7b7b7b},
    {Gray,        ANSI{24}, 0x4a4a4a},
    {Red,         ANSI{25}, 0xc46c71},
    {Light_green, ANSI{26}, 0xa9ff9f},
    {Yellow,      ANSI{27}, 0xedf171},
    {Light_blue,  ANSI{28}, 0x706deb},
    {Orange,      ANSI{29}, 0x8e5029},
    {Silver,      ANSI{30}, 0xb2b2b2},
    {White,       ANSI{31}, 0xffffff},
    /* clang-format on */
};

}  // namespace ox::commodore_64
#endif  // TERMOX_PAINTER_PALETTE_COMMODORE_64_HPP
