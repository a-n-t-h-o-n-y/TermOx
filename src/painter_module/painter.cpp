#include <mcurses/painter_module/painter.hpp>
#include <mcurses/painter_module/color.hpp>

namespace mcurses {

void
Painter::move(unsigned x, unsigned y)
{
	unsigned glob_x = widget_->global_x() + x;
	unsigned glob_y = widget_->global_y() + y;

	if(!widget_->has_coordinates(glob_x, glob_y)) {
		// There is no level above this to handle is, just disregarg and do not move
		// maybe create a helper function that all functions in class can use
		// to verify_coordinates();
	} else {
		widget_->paint_engine().move(glob_x, glob_y);
	}
	return;
}

void
Painter::put(char c)
{
	widget_->paint_engine().put_char(c);
	return;
}

void
Painter::put(const std::string& s)
{
	widget_->paint_engine().put_string(s);
	return;
}

void Painter::fill(unsigned x, unsigned y, unsigned width, unsigned height, Color background)
{
	widget_->paint_engine().fill_rect(x, y, width, height, background);
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

} // namespace mcurses