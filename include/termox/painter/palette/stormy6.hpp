#ifndef TERMOX_PAINTER_PALETTE_STORMY6_HPP
#define TERMOX_PAINTER_PALETTE_STORMY6_HPP
#include <termox/painter/color.hpp>

namespace ox::stormy6 {

constexpr auto Black  = Color::Background;
constexpr auto Green  = Color{2};
constexpr auto Teal   = Color{7};
constexpr auto Red    = Color{9};
constexpr auto Orange = Color{13};
constexpr auto White  = Color::Foreground;

/// Stormy 6 Color Palette
/** https://lospec.com/palette-list/stormy-6 */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,  ANSI{16}, 0x242828},
    {Green,  ANSI{17}, 0x7f9860},
    {Teal,   ANSI{18}, 0x3a5043},
    {Red,    ANSI{19}, 0xa95a3f},
    {Orange, ANSI{20}, 0xedbb70},
    {White,  ANSI{21}, 0xf8eebf},
    /* clang-format on */
};

}  // namespace ox::stormy6
#endif  // TERMOX_PAINTER_PALETTE_STORMY6_HPP
