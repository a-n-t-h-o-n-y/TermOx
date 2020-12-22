#ifndef TERMOX_PAINTER_PALETTE_MSX_HPP
#define TERMOX_PAINTER_PALETTE_MSX_HPP
#include <termox/painter/color.hpp>

namespace ox::msx {

constexpr auto Black       = Color::Background;
constexpr auto Dark_red    = Color{1};
constexpr auto Dark_green  = Color{2};
constexpr auto Olive       = Color{3};
constexpr auto Blue        = Color{4};
constexpr auto Violet      = Color{5};
constexpr auto Cyan        = Color{6};
constexpr auto Silver      = Color{7};
constexpr auto Indigo      = Color{8};
constexpr auto Red         = Color{9};
constexpr auto Light_green = Color{10};
constexpr auto Brown       = Color{11};
constexpr auto Light_red   = Color{12};
constexpr auto Green       = Color{13};
constexpr auto White       = Color::Foreground;

/// MSX Home Computer 15 Color Palette
/** https://lospec.com/palette-list/msx */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,       ANSI{16}, 0x000000},
    {Dark_red,    ANSI{17}, 0xb75e51},
    {Dark_green,  ANSI{18}, 0x3ca042},
    {Olive,       ANSI{19}, 0xcac15e},
    {Blue,        ANSI{20}, 0x5955df},
    {Violet,      ANSI{21}, 0xb565b3},
    {Cyan,        ANSI{22}, 0x64daee},
    {Silver,      ANSI{23}, 0xcacaca},
    {Indigo,      ANSI{24}, 0x7e75f0},
    {Red,         ANSI{25}, 0xd96459},
    {Light_green, ANSI{26}, 0x73ce7c},
    {Brown,       ANSI{27}, 0xddce85},
    {Light_red,   ANSI{28}, 0xfe877c},
    {Green,       ANSI{29}, 0x40b64a},
    {White,       ANSI{30}, 0xffffff},
    /* clang-format on */
};

}  // namespace ox::msx
#endif  // TERMOX_PAINTER_PALETTE_MSX_HPP
