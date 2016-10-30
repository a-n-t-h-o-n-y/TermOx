#ifndef PAINT_ENGINE_HPP
#define PAINT_ENGINE_HPP

#include "color.hpp"
#include "attribute.hpp"

#include <string>

namespace mcurses {

class Paint_engine {
public:
	virtual void refresh() = 0;
	virtual void move(unsigned x, unsigned y) = 0;
	// virtual void put(const Glyph_string& gs) = 0;
	virtual void put_char(char c) = 0; // delete later
	virtual void put_string(const std::string& s) = 0; // delete later
	virtual void show_cursor() = 0;
	virtual void hide_cursor() = 0;
	virtual unsigned screen_width() = 0;
	virtual unsigned screen_height() = 0;
	virtual void set_rgb(Color c, int r, int g, int b) = 0;
	virtual void set_attribute(Attribute attr) = 0;
	virtual void unset_attribute(Attribute attr) = 0;
	virtual void clear_attributes() = 0;
	virtual void set_background_color(Color c) = 0;
	virtual void set_foreground_color(Color c) = 0;

protected:
	
};

} // namespace mcurses

#endif // PAINT_ENGINE_HPP