#ifndef TERMOX_PAINTER_PALETTE_ASHES_HPP
#define TERMOX_PAINTER_PALETTE_ASHES_HPP
#include <termox/painter/color.hpp>

namespace ox::ashes {

inline constexpr auto Black       = Color::Background;
inline constexpr auto Red         = Color{1};
inline constexpr auto Green       = Color{2};
inline constexpr auto Brown       = Color{3};
inline constexpr auto Blue        = Color{4};
inline constexpr auto Violet      = Color{5};
inline constexpr auto Light_green = Color{6};
inline constexpr auto Silver      = Color{7};
inline constexpr auto Gray        = Color{8};
inline constexpr auto White       = Color::Foreground;

/// Ashes scheme by Jannik Siebert (https://github.com/janniks)
inline auto const palette = Palette{
    /* clang-format off */
    {Black,       RGB{0x1c2023}},
    {Red,         RGB{0xc795ae}},
    {Green,       RGB{0x95c7ae}},
    {Brown,       RGB{0xc7ae95}},
    {Blue,        RGB{0x95aec7}},
    {Violet,      RGB{0xae95c7}},
    {Light_green, RGB{0xaec795}},
    {Silver,      RGB{0xc7ccd1}},
    {Gray,        RGB{0x747c84}},
    {White,       RGB{0xf3f4f5}},
    /* clang-format on */
};

}  // namespace ox::ashes
#endif  // TERMOX_PAINTER_PALETTE_ASHES_HPP
