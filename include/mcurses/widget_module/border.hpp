#ifndef BORDER_HPP
#define BORDER_HPP

#include "../painter_module/palette.hpp"
#include "../painter_module/glyph.hpp"

#include <string>

namespace mcurses {
class Widget;

class Border {
public:
	Border(Widget* w): widget_{w}{}

	void set_walls(const Glyph& horizontals, const Glyph& verticals);
	void set_north(const Glyph& g) { north_ = g; }
	void set_south(const Glyph& g) { south_ = g; }
	void set_east(const Glyph& g) { east_ = g; }
	void set_west(const Glyph& g) { west_ = g; }
	void set_north_west(const Glyph& g) { north_west_ = g; }
	void set_north_east(const Glyph& g) { north_east_ = g; }
	void set_south_west(const Glyph& g) { south_west_ = g; }
	void set_south_east(const Glyph& g) { south_east_ = g; }

	void draw() const;

	void set_parent(Widget* w) { widget_ = w; }

	bool is_enabled() const { return enabled_; }
	void enable() { enabled_ = true; }
	void disable() { enabled_ = false; }

private:
	Glyph north_ = "─";
	Glyph south_ = "─";
	Glyph east_ = "│";
	Glyph west_ = "│";
	Glyph north_west_ = "┌";
	Glyph north_east_ = "┐";
	Glyph south_west_ = "└";
	Glyph south_east_ = "┘";

	Palette palette_;
	bool enabled_ = false;
	Widget* widget_ = nullptr;
};

} // namespace mcurses
#endif // BORDER_HPP