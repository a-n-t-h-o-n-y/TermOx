#ifndef WIDGET_MODULE_BORDER_HPP
#define WIDGET_MODULE_BORDER_HPP

#include "painter_module/glyph.hpp"
#include <cstddef>

namespace twf {
class Widget;

class Border {
   public:
    void set_walls(const Glyph& horizontals, const Glyph& verticals);
    void set_north(const Glyph& g) { north_ = g; }
    void set_south(const Glyph& g) { south_ = g; }
    void set_east(const Glyph& g) { east_ = g; }
    void set_west(const Glyph& g) { west_ = g; }
    void set_north_west(const Glyph& g) { north_west_ = g; }
    void set_north_east(const Glyph& g) { north_east_ = g; }
    void set_south_west(const Glyph& g) { south_west_ = g; }
    void set_south_east(const Glyph& g) { south_east_ = g; }

    Glyph north() const { return north_; }
    Glyph south() const { return south_; }
    Glyph east() const { return east_; }
    Glyph west() const { return west_; }
    Glyph north_west() const { return north_west_; }
    Glyph north_east() const { return north_east_; }
    Glyph south_west() const { return south_west_; }
    Glyph south_east() const { return south_east_; }

    bool enabled() const { return enabled_; }
    void enable() { enabled_ = true; }
    void disable() { enabled_ = false; }

    void enable_walls();
    void disable_walls();
    void enable_corners();
    void disable_corners();

    void enable_north() { north_enabled_ = true; }
    void enable_south() { south_enabled_ = true; }
    void enable_west() { west_enabled_ = true; }
    void enable_east() { east_enabled_ = true; }
    void enable_north_west() { north_west_enabled_ = true; }
    void enable_north_east() { north_east_enabled_ = true; }
    void enable_south_west() { south_west_enabled_ = true; }
    void enable_south_east() { south_east_enabled_ = true; }

    void disable_north() { north_enabled_ = false; }
    void disable_south() { south_enabled_ = false; }
    void disable_west() { west_enabled_ = false; }
    void disable_east() { east_enabled_ = false; }
    void disable_north_west() { north_west_enabled_ = false; }
    void disable_north_east() { north_east_enabled_ = false; }
    void disable_south_west() { south_west_enabled_ = false; }
    void disable_south_east() { south_east_enabled_ = false; }

    bool north_enabled() const { return north_enabled_; }
    bool south_enabled() const { return south_enabled_; }
    bool east_enabled() const { return east_enabled_; }
    bool west_enabled() const { return west_enabled_; }
    bool north_west_enabled() const { return north_west_enabled_; }
    bool north_east_enabled() const { return north_east_enabled_; }
    bool south_west_enabled() const { return south_west_enabled_; }
    bool south_east_enabled() const { return south_east_enabled_; }

   private:
    Glyph north_ = "─";
    Glyph south_ = "─";
    Glyph east_ = "│";
    Glyph west_ = "│";
    Glyph north_west_ = "┌";
    Glyph north_east_ = "┐";
    Glyph south_west_ = "└";
    Glyph south_east_ = "┘";

    bool enabled_ = false;

    bool north_enabled_ = true;
    bool south_enabled_ = true;
    bool east_enabled_ = true;
    bool west_enabled_ = true;
    bool north_west_enabled_ = true;
    bool north_east_enabled_ = true;
    bool south_west_enabled_ = true;
    bool south_east_enabled_ = true;
};

std::size_t west_border_offset(const Border& b);
std::size_t east_border_offset(const Border& b);
std::size_t north_border_offset(const Border& b);
std::size_t south_border_offset(const Border& b);

}  // namespace twf
#endif  // WIDGET_MODULE_BORDER_HPP
