#ifndef CPPURSES_PAINTER_PALETTE_EN4_HPP
#define CPPURSES_PAINTER_PALETTE_EN4_HPP
#include <array>
#include <cstdint>

#include <cppurses/painter/color.hpp>

namespace cppurses::en4 {

constexpr auto Blue  = Color{0};
constexpr auto Peach = Color{1};
constexpr auto Green = Color{2};
constexpr auto White = Color{3};

inline auto const palette = Color_palette{
    /* clang-format off */
    {Blue,  ANSI{16}, True_color{0x20283d}},
    {Peach, ANSI{17}, True_color{0xe5b083}},
    {Green, ANSI{18}, True_color{0x426e5d}},
    {White, ANSI{19}, True_color{0xfbf7f3}},
    /* clang-format on */
};

}  // namespace cppurses::en4
#endif  // CPPURSES_PAINTER_PALETTE_EN4_HPP
