#ifndef TERMOX_PAINTER_PALETTE_MSX_HPP
#define TERMOX_PAINTER_PALETTE_MSX_HPP
#include <termox/painter/color.hpp>

namespace ox::msx {

inline constexpr auto Black       = Color::Background;
inline constexpr auto Dark_red    = Color{1};
inline constexpr auto Dark_green  = Color{2};
inline constexpr auto Olive       = Color{3};
inline constexpr auto Blue        = Color{4};
inline constexpr auto Violet      = Color{5};
inline constexpr auto Cyan        = Color{6};
inline constexpr auto Silver      = Color{7};
inline constexpr auto Indigo      = Color{8};
inline constexpr auto Red         = Color{9};
inline constexpr auto Light_green = Color{10};
inline constexpr auto Brown       = Color{11};
inline constexpr auto Light_red   = Color{12};
inline constexpr auto Green       = Color{13};
inline constexpr auto White       = Color::Foreground;

/// MSX Home Computer 15 Color Palette
/** https://lospec.com/palette-list/msx */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,       RGB{0x000000}},
    {Dark_red,    RGB{0xb75e51}},
    {Dark_green,  RGB{0x3ca042}},
    {Olive,       RGB{0xcac15e}},
    {Blue,        RGB{0x5955df}},
    {Violet,      RGB{0xb565b3}},
    {Cyan,        RGB{0x64daee}},
    {Silver,      RGB{0xcacaca}},
    {Indigo,      RGB{0x7e75f0}},
    {Red,         RGB{0xd96459}},
    {Light_green, RGB{0x73ce7c}},
    {Brown,       RGB{0xddce85}},
    {Light_red,   RGB{0xfe877c}},
    {Green,       RGB{0x40b64a}},
    {White,       RGB{0xffffff}},
    /* clang-format on */
};

}  // namespace ox::msx
#endif  // TERMOX_PAINTER_PALETTE_MSX_HPP
