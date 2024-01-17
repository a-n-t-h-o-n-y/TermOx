#ifndef TERMOX_PAINTER_PALETTE_SAVANNA_HPP
#define TERMOX_PAINTER_PALETTE_SAVANNA_HPP
#include <termox/painter/color.hpp>

namespace ox::savanna {

inline constexpr auto Black  = Color::Background;
inline constexpr auto Rust   = Color{1};
inline constexpr auto Green  = Color{2};
inline constexpr auto Brown  = Color{3};
inline constexpr auto Blue   = Color{4};
inline constexpr auto Violet = Color{5};
inline constexpr auto Cyan   = Color{6};
inline constexpr auto Silver = Color{7};
inline constexpr auto Gray   = Color{8};
inline constexpr auto White  = Color::Foreground;

/// Atelier Savanna scheme by Bram de Haan (http://atelierbramdehaan.nl)
inline auto const palette = Palette{
    /* clang-format off */
    {Black,  RGB{0x171c19}},
    {Rust,   RGB{0xb16139}},
    {Green,  RGB{0x489963}},
    {Brown,  RGB{0xa07e3b}},
    {Blue,   RGB{0x478c90}},
    {Violet, RGB{0x55859b}},
    {Cyan,   RGB{0x1c9aa0}},
    {Silver, RGB{0x78877d}},
    {Gray,   RGB{0x526057}},
    {White,  RGB{0xecf4ee}},
    /* clang-format on */
};

}  // namespace ox::savanna
#endif  // TERMOX_PAINTER_PALETTE_SAVANNA_HPP
