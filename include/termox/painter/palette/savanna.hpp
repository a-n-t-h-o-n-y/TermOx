#ifndef TERMOX_PAINTER_PALETTE_SAVANNA_HPP
#define TERMOX_PAINTER_PALETTE_SAVANNA_HPP
#include <termox/painter/color.hpp>

namespace ox::savanna {

constexpr auto Black  = Color::Background;
constexpr auto Rust   = Color{1};
constexpr auto Green  = Color{2};
constexpr auto Brown  = Color{3};
constexpr auto Blue   = Color{4};
constexpr auto Violet = Color{5};
constexpr auto Cyan   = Color{6};
constexpr auto Silver = Color{7};
constexpr auto Gray   = Color{8};
constexpr auto White  = Color::Foreground;

/// Atelier Savanna scheme by Bram de Haan (http://atelierbramdehaan.nl)
inline auto const palette = Palette{
    /* clang-format off */
    {Black,  ANSI{16}, 0x171c19},
    {Rust,   ANSI{17}, 0xb16139},
    {Green,  ANSI{18}, 0x489963},
    {Brown,  ANSI{19}, 0xa07e3b},
    {Blue,   ANSI{20}, 0x478c90},
    {Violet, ANSI{21}, 0x55859b},
    {Cyan,   ANSI{22}, 0x1c9aa0},
    {Silver, ANSI{23}, 0x78877d},
    {Gray,   ANSI{24}, 0x526057},
    {White,  ANSI{25}, 0xecf4ee},
    /* clang-format on */
};

}  // namespace ox::savanna
#endif  // TERMOX_PAINTER_PALETTE_SAVANNA_HPP
