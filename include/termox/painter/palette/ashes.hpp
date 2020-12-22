#ifndef TERMOX_PAINTER_PALETTE_ASHES_HPP
#define TERMOX_PAINTER_PALETTE_ASHES_HPP
#include <termox/painter/color.hpp>

namespace ox::ashes {

constexpr auto Black       = Color::Background;
constexpr auto Red         = Color{1};
constexpr auto Green       = Color{2};
constexpr auto Brown       = Color{3};
constexpr auto Blue        = Color{4};
constexpr auto Violet      = Color{5};
constexpr auto Light_green = Color{6};
constexpr auto Silver      = Color{7};
constexpr auto Gray        = Color{8};
constexpr auto White       = Color::Foreground;

/// Ashes scheme by Jannik Siebert (https://github.com/janniks)
inline auto const palette = Palette{
    /* clang-format off */
    {Black,       ANSI{16}, 0x1c2023},
    {Red,         ANSI{17}, 0xc795ae},
    {Green,       ANSI{18}, 0x95c7ae},
    {Brown,       ANSI{19}, 0xc7ae95},
    {Blue,        ANSI{20}, 0x95aec7},
    {Violet,      ANSI{21}, 0xae95c7},
    {Light_green, ANSI{22}, 0xaec795},
    {Silver,      ANSI{23}, 0xc7ccd1},
    {Gray,        ANSI{24}, 0x747c84},
    {White,       ANSI{25}, 0xf3f4f5},
    /* clang-format on */
};

}  // namespace ox::ashes
#endif  // TERMOX_PAINTER_PALETTE_ASHES_HPP
