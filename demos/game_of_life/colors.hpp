#ifndef TERMOX_DEMOS_GAME_OF_LIFE_COLORS_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_COLORS_HPP
#include <termox/painter/color.hpp>
#include <termox/painter/dynamic_colors.hpp>

namespace gol {
namespace color {

inline auto constexpr Black       = ox::Color::Background;
inline auto constexpr Light_green = ox::Color{1};
inline auto constexpr Green       = ox::Color{2};
inline auto constexpr Teal        = ox::Color{3};
inline auto constexpr Rainbow     = ox::Color{4};
inline auto constexpr White       = ox::Color::Foreground;

}  // namespace color

inline auto const gol_palette = ox::Palette{
    // clang-format off
    {color::Black,       ox::RGB{0x0c2e44}},
    {color::Light_green, ox::RGB{0x5ac54f}},
    {color::Green,       ox::RGB{0x33984b}},
    {color::Teal,        ox::RGB{0x134c4c}},
    {color::Rainbow,
        ox::dynamic::rainbow(std::chrono::milliseconds{40}, 75, 70)},
    {color::White,       ox::RGB{0xf9e6cf}},
    // clang-format on
};

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_COLORS_HPP
