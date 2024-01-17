#ifndef TERMOX_PAINTER_PALETTE_EN4_HPP
#define TERMOX_PAINTER_PALETTE_EN4_HPP
#include <termox/painter/color.hpp>

namespace ox::en4 {

inline constexpr auto Blue  = Color::Background;
inline constexpr auto Peach = Color{5};
inline constexpr auto Green = Color{2};
inline constexpr auto White = Color::Foreground;

/// EN 4 Color Palette
/** https://lospec.com/palette-list/en4 */
inline auto const palette = Palette{
    /* clang-format off */
    {Blue,  RGB{0x20283d}},
    {Peach, RGB{0xe5b083}},
    {Green, RGB{0x426e5d}},
    {White, RGB{0xfbf7f3}},
    /* clang-format on */
};

}  // namespace ox::en4
#endif  // TERMOX_PAINTER_PALETTE_EN4_HPP
