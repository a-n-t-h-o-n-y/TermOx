#ifndef TERMOX_TERMINAL_OUTPUT_HPP
#define TERMOX_TERMINAL_OUTPUT_HPP
#include <cstddef>

#include <termox/painter/glyph.hpp>

namespace ox::output {

/// Moves the cursor the point \p x , \p y on screen.
/** (0,0) is top left of the terminal screen. */
void move_cursor(std::size_t x, std::size_t y);

/// Flushes all of the changes made since the last refresh to the screen.
void refresh();

/// Places Glyph \p g on the screen at the current cursor position.
void put(Glyph g);

/// Places Glyph \p g at coordinates \p x , \p y.
/** First moves the cursor's position to (x,y), then puts the Glyph to the
 *  screen. (0,0) is top left of the terminal screen.*/
inline void put(std::size_t x, std::size_t y, const Glyph& g)
{
    move_cursor(x, y);
    put(g);
}

}  // namespace ox::output
#endif  // TERMOX_TERMINAL_OUTPUT_HPP
