#ifndef PAINTER_HPP
#define PAINTER_HPP

#include "../widget_module/widget.hpp"
#include "color.hpp"
#include "glyph_string.hpp"

#include <string>
#include <sstream>
#include <codecvt>
#include <locale>


namespace mcurses {
class Border;

class Painter {
public:
	Painter(Widget* widget);

	// Essential Functions
	void put(const Glyph_string& gs);
	void move(unsigned x, unsigned y);
	
	// Convinience functions
	void fill(unsigned x, unsigned y, unsigned width, unsigned height, Color fill_background);
	void line(unsigned x1, unsigned y1, unsigned x2, unsigned y2, const Glyph_string& gs = "-");
	void border(const Border& b);
	void border(const Glyph_string& gs);

	void set_cursor(bool state);

	// void refresh(); // should not need this

private:
	// Functions that do not change widget's cursor position
	// void move_(unsigned x, unsigned y);
	// void put_(const Glyph_string& gs);
	// void put_glyph_(const Glyph& g); // prob don't need

	Widget* widget_;
};

} // namespace mcurses

#endif // PAINTER_HPP