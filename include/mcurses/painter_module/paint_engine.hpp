#ifndef PAINT_ENGINE_HPP
#define PAINT_ENGINE_HPP

#include "color.hpp"

#include <string>

namespace mcurses {

class Paint_engine {
public:
	virtual void move(unsigned x, unsigned y) = 0;
	virtual void put_char(char c) = 0;
	virtual void put_string(const std::string& s) = 0;
	virtual void show_cursor() = 0;
	virtual void hide_cursor() = 0;
	virtual unsigned screen_width() = 0;
	virtual unsigned screen_height() = 0;
	virtual void fill_rect(unsigned x, unsigned y, unsigned width, unsigned height, Color background) = 0;

protected:
	
};

} // namespace mcurses

#endif // PAINT_ENGINE_HPP