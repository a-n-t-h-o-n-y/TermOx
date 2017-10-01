#ifndef WIDGET_BORDER_HPP
#define WIDGET_BORDER_HPP
#include "painter/glyph.hpp"
#include <cstddef>

namespace cppurses {

struct Border {
   public:
    bool enabled = false;

    Glyph north = "─";
    Glyph south = "─";
    Glyph east = "│";
    Glyph west = "│";
    Glyph north_west = "┌";
    Glyph north_east = "┐";
    Glyph south_west = "└";
    Glyph south_east = "┘";

    bool north_enabled = true;
    bool south_enabled = true;
    bool east_enabled = true;
    bool west_enabled = true;
    bool north_west_enabled = true;
    bool north_east_enabled = true;
    bool south_west_enabled = true;
    bool south_east_enabled = true;
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

std::size_t west_border_offset(const Border& b);
std::size_t east_border_offset(const Border& b);
std::size_t north_border_offset(const Border& b);
std::size_t south_border_offset(const Border& b);

}  // namespace cppurses
#endif  // WIDGET_BORDER_HPP
