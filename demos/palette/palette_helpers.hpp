#ifndef CPPURSES_DEMOS_PALETTE_PALETTE_HELPERS_HPP
#define CPPURSES_DEMOS_PALETTE_PALETTE_HELPERS_HPP
#include <cstdint>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/color_definition.hpp>
#include <cppurses/painter/palette.hpp>

namespace palette {
namespace detail {

/// Return the current Color_definition of \p color in \p palette.
cppurses::Color_definition& extract_definition(cppurses::Palette& palette,
                                               cppurses::Color color);

/// Set the red value of \p color and update the display.
void set_red(cppurses::Color color, std::int16_t red_value);

/// Set the green value of \p color and update the display.
void set_green(cppurses::Color color, std::int16_t green_value);

/// Set the blue value of \p color and update the display.
void set_blue(cppurses::Color color, std::int16_t blue_value);

}  // namespace detail
}  // namespace palette
#endif  // CPPURSES_DEMOS_PALETTE_PALETTE_HELPERS_HPP
