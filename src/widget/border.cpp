#include "widget/border.hpp"
#include "painter/glyph.hpp"
#include <cstddef>

namespace cppurses {

void set_walls(Border& b, const Glyph& horizontal, const Glyph& vertical) {
    b.north = horizontal;
    b.south = horizontal;
    b.east = vertical;
    b.west = vertical;
}

void enable_walls(Border& b) {
    b.north_enabled = true;
    b.south_enabled = true;
    b.west_enabled = true;
    b.east_enabled = true;
}

void disable_walls(Border& b) {
    b.north_enabled = false;
    b.south_enabled = false;
    b.west_enabled = false;
    b.east_enabled = false;
}

void set_corners(Border& b,
                 const Glyph& nw,
                 const Glyph& ne,
                 const Glyph& sw,
                 const Glyph& se) {
    b.north_west = nw;
    b.north_east = ne;
    b.south_west = sw;
    b.south_east = se;
}

void enable_corners(Border& b) {
    b.north_west_enabled = true;
    b.north_east_enabled = true;
    b.south_west_enabled = true;
    b.south_east_enabled = true;
}

void disable_corners(Border& b) {
    b.north_west_enabled = false;
    b.north_east_enabled = false;
    b.south_west_enabled = false;
    b.south_east_enabled = false;
}

std::size_t west_border_offset(const Border& b) {
    if (b.enabled &&
        (b.west_enabled || b.north_west_enabled || b.south_west_enabled)) {
        return 1;
    }
    return 0;
}

std::size_t east_border_offset(const Border& b) {
    if (b.enabled &&
        (b.east_enabled || b.north_east_enabled || b.south_east_enabled)) {
        return 1;
    }
    return 0;
}

std::size_t north_border_offset(const Border& b) {
    if (b.enabled &&
        (b.north_enabled || b.north_east_enabled || b.north_west_enabled)) {
        return 1;
    }
    return 0;
}

std::size_t south_border_offset(const Border& b) {
    if (b.enabled &&
        (b.south_enabled || b.south_east_enabled || b.south_west_enabled)) {
        return 1;
    }
    return 0;
}

}  // namespace cppurses
