#ifndef CPPURSES_PAINTER_PALETTE_EN4_HPP
#define CPPURSES_PAINTER_PALETTE_EN4_HPP
#include <array>
#include <cstdint>

#include <cppurses/painter/color.hpp>

namespace cppurses::en4 {

constexpr auto White = Color{0};
constexpr auto Peach = Color{1};
constexpr auto Green = Color{2};
constexpr auto Blue  = Color{3};

/* clang-format off */
inline auto const palette = True_color_palette{{
    {{White, ANSI{0}}, True_color{0xfbf7f3}},
    {{Peach, ANSI{0}}, True_color{0xe5b083}},
    {{Green, ANSI{0}}, True_color{0x426e5d}},
    {{Blue,  ANSI{0}}, True_color{0x20283d}},
}};
/* clang-format on */
}  // namespace cppurses::en4
#endif  // CPPURSES_PAINTER_PALETTE_EN4_HPP
