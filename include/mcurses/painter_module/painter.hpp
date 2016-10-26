#ifndef PAINTER_HPP
#define PAINTER_HPP

#include "../widget_module/widget.hpp"
#include "color.hpp"

#include <string>

namespace mcurses {

class Painter {
public:
	Painter(Widget* widget):widget_{widget}
	{
		this->show_cursor(widget_->show_cursor());
	}
	void move(unsigned x, unsigned y);
	void put(char c);
	void put(const std::string& s);
	void fill(unsigned x, unsigned y, unsigned width, unsigned height, Color background);
	void show_cursor(bool state);


private:
	Widget* widget_;
};

} // namespace mcurses

#endif // PAINTER_HPP