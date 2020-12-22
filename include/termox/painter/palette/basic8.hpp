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
    {Black,   ANSI{0}},
    {Maroon,  ANSI{1}},
    {Green,   ANSI{2}},
    {Olive,   ANSI{3}},
    {Navy,    ANSI{4}},
    {Purple,  ANSI{5}},
    {Teal,    ANSI{6}},
    {Silver,  ANSI{7}},
    /* clang-format on */
};

}  // namespace ox::basic8
#endif  // TERMOX_PAINTER_PALETTE_BASIC8_HPP
