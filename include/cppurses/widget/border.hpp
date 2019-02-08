#ifndef CPPURSES_WIDGET_BORDER_HPP
#define CPPURSES_WIDGET_BORDER_HPP
#include <cppurses/painter/glyph.hpp>

namespace cppurses {

/// Holds the look of a Widget's Border.
struct Border {
    bool enabled{false};

    // TODO Can combine single Glyph and bool into an object(struct)
    Glyph north{L'─'};
    Glyph south{L'─'};
    Glyph east{L'│'};
    Glyph west{L'│'};
    Glyph north_west{L'┌'};
    Glyph north_east{L'┐'};
    Glyph south_west{L'└'};
    Glyph south_east{L'┘'};

    bool north_enabled{true};
    bool south_enabled{true};
    bool east_enabled{true};
    bool west_enabled{true};
    bool north_west_enabled{true};
    bool north_east_enabled{true};
    bool south_west_enabled{true};
    bool south_east_enabled{true};
};

/**
 * @brief Change the Glyph for each the horizontal and vertical walls.
 *
 * @param b Border to be modified.
 * @param horizontal Glyph used for each space on the horizontal wall.
 * @param vertical Glyph used for each space on the vertical wall.
 */
void set_walls(Border& b, const Glyph& horizontal, const Glyph& vertical);

/// Enable all walls of the border to display.
void enable_walls(Border& b);

/// Disable all walls of the border.
void disable_walls(Border& b);

/// Set the Glyphs to be used by each of the corners of the Border.
void set_corners(Border& b,
                 const Glyph& nw,
                 const Glyph& ne,
                 const Glyph& sw,
                 const Glyph& se);

/// Enable all four corners of the Border to be displayed.
void enable_corners(Border& b);

/// Disable all four corners of the Border.
void disable_corners(Border& b);

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_BORDER_HPP
