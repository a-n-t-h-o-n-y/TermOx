#ifndef PAINTER_HPP
#define PAINTER_HPP

#include "canvas.hpp"

#include <string>

namespace mcurses {

class Painter {
public:
	Painter(Canvas* widget):canvas_{widget}
	{
		this->show_cursor(canvas_->show_cursor());
	}
	void move(unsigned x, unsigned y);
	void put(char c);
	void put(const std::string& s);
	void show_cursor(bool state);

private:
	Canvas* canvas_;
};

} // namespace mcurses

#endif // PAINTER_HPP