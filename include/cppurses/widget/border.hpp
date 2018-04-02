#ifndef WIDGET_BORDER_HPP
#define WIDGET_BORDER_HPP
#include <cppurses/painter/glyph.hpp>

namespace cppurses {

struct Border {
    bool enabled{false};

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

void set_walls(Border& b, const Glyph& horizontal, const Glyph& vertical);
void enable_walls(Border& b);
void disable_walls(Border& b);

void set_corners(Border& b,
                 const Glyph& nw,
                 const Glyph& ne,
                 const Glyph& sw,
                 const Glyph& se);
void enable_corners(Border& b);
void disable_corners(Border& b);

}  // namespace cppurses
#endif  // WIDGET_BORDER_HPP
