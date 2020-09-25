#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_COLORS_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_COLORS_HPP
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/dynamic_colors.hpp>

namespace gol {
namespace color {

inline auto constexpr Black       = cppurses::Color::Background;
inline auto constexpr Light_green = cppurses::Color{1};
inline auto constexpr Green       = cppurses::Color{2};
inline auto constexpr Teal        = cppurses::Color{3};
inline auto constexpr Rainbow     = cppurses::Color{4};
inline auto constexpr White       = cppurses::Color::Foreground;

}  // namespace color

inline auto const gol_palette = cppurses::Color_palette{
    /* clang-format off */
        {color::Black,       cppurses::ANSI{16}, 0x0c2e44},
        {color::Light_green, cppurses::ANSI{17}, 0x5ac54f},
        {color::Green,       cppurses::ANSI{18}, 0x33984b},
        {color::Teal,        cppurses::ANSI{19}, 0x134c4c},
        {color::Rainbow,     cppurses::ANSI{20},
           cppurses::dynamic::rainbow(std::chrono::milliseconds{40}, 75, 70)},
        {color::White,       cppurses::ANSI{21}, 0xf9e6cf},
    /* clang-format on */
};

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_COLORS_HPP
