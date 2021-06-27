#ifndef TERMOX_PAINTER_PALETTE_BASIC8_HPP
#define TERMOX_PAINTER_PALETTE_BASIC8_HPP
#include <termox/painter/color.hpp>

namespace ox::basic8 {

constexpr auto Black  = Color::Background;
constexpr auto Maroon = Color{1};
constexpr auto Green  = Color{2};
constexpr auto Olive  = Color{3};
constexpr auto Navy   = Color{4};
constexpr auto Purple = Color{5};
constexpr auto Teal   = Color{6};
constexpr auto Silver = Color::Foreground;

/// The Built-in Default Terminal 8 Color Palette
inline auto const palette = Palette{
    /* clang-format off */
    {Black,   Color_index{0}},
    {Maroon,  Color_index{1}},
    {Green,   Color_index{2}},
    {Olive,   Color_index{3}},
    {Navy,    Color_index{4}},
    {Purple,  Color_index{5}},
    {Teal,    Color_index{6}},
    {Silver,  Color_index{7}},
    /* clang-format on */
};

}  // namespace ox::basic8
#endif  // TERMOX_PAINTER_PALETTE_BASIC8_HPP
