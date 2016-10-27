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
		this->move(0,0);
	}
	void refresh();
	void move(unsigned x, unsigned y);
	void put(char c);
	void put(const std::string& s);
	void fill(unsigned x, unsigned y, unsigned width, unsigned height, Color background);
	void show_cursor(bool state);
	void line(unsigned x1, unsigned y1, unsigned x2, unsigned y2, const std::string& b = "-");
	void border(const std::string& b);


private:
	Widget* widget_;
	bool valid_position_ = true;
};

} // namespace mcurses

#endif // PAINTER_HPP