#include <mcurses/painter_module/painter.hpp>
#include <mcurses/painter_module/color.hpp>

namespace mcurses {

void
Painter::refresh()
{
	widget_->paint_engine().refresh();
}

void
Painter::move(unsigned x, unsigned y)
{
	unsigned glob_x = widget_->global_x() + x;
	unsigned glob_y = widget_->global_y() + y;

	if(!widget_->has_coordinates(glob_x, glob_y)) {
		valid_position_ = false;
	} else {
		valid_position_ = true;
		widget_->paint_engine().move(glob_x, glob_y);
	}
	return;
}

void
Painter::put(char c)
{
	if(!valid_position_) { return; }
	widget_->paint_engine().put_char(c);
	return;
}

void
Painter::put(const std::string& s)
{
	if(!valid_position_) { return; }
	widget_->paint_engine().put_string(s);
	return;
}

void Painter::fill(unsigned x, unsigned y, unsigned width, unsigned height, Color background)
{
	for(unsigned i{y}; i < height; ++i) {
		this->line(x, i, width, i, " ");	// eventually update with proper background color
	}
	return;
}

void
Painter::show_cursor(bool state)
{
	if(state) {
		widget_->paint_engine().show_cursor();
	} else {
		widget_->paint_engine().hide_cursor();
	}
	return;
}

void Painter::line(unsigned x1, unsigned y1, unsigned x2, unsigned y2, const std::string& b)
{
	// points must be left to right/ top to bottom right now, implement alternatives.
	// Horizontal
	if(y1 == y2) {
		for(unsigned i{x1}; i <= x2; ++i) {
			this->move(i, y1);
			this->put(b);
		}
	}
	// Vertical
	else if(x1 == x2) {
		for(unsigned i{y1}; i <= y2; ++i) {
			this->move(x1, i);
			this->put(b);
		}
	}
	// Diagonal not implemented right now
	return;
}

void Painter::border(const std::string& b)
{
	// Top border
	this->line(0, 0, widget_->width()-1, 0, b);

	// Bottom border
	this->line(0, widget_->height()-1, widget_->width()-1, widget_->height()-1, b);

	// Middle sides
	this->line(0, 0, 0, widget_->height()-1, b);
	this->line(widget_->width()-1, 0, widget_->width()-1, widget_->height()-1, b);
}

} // namespace mcurses