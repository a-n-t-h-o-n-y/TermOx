#ifndef TERMOX_PAINTER_PALETTE_STORMY6_HPP
#define TERMOX_PAINTER_PALETTE_STORMY6_HPP
#include <termox/painter/color.hpp>

namespace ox::stormy6 {

inline constexpr auto Black  = Color::Background;
inline constexpr auto Green  = Color{2};
inline constexpr auto Teal   = Color{7};
inline constexpr auto Red    = Color{9};
inline constexpr auto Orange = Color{13};
inline constexpr auto White  = Color::Foreground;

/// Stormy 6 Color Palette
/** https://lospec.com/palette-list/stormy-6 */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,  RGB{0x242828}},
    {Green,  RGB{0x7f9860}},
    {Teal,   RGB{0x3a5043}},
    {Red,    RGB{0xa95a3f}},
    {Orange, RGB{0xedbb70}},
    {White,  RGB{0xf8eebf}},
    /* clang-format on */
};

}  // namespace ox::stormy6
#endif  // TERMOX_PAINTER_PALETTE_STORMY6_HPP
