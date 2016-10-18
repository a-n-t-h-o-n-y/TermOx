#include <mcurses/painter_module/painter.hpp>

namespace mcurses {

void
Painter::move(unsigned x, unsigned y)
{
	unsigned glob_x = canvas_->position_x() + x;
	unsigned glob_y = canvas_->position_y() + y;

	if(!canvas_->check_coordinates(glob_x, glob_y)) {
		// some kind of error; throw exception(out of bounds widget paint requested)
	} else {
		canvas_->engine().move(glob_x, glob_y);
	}
	return;
}

void
Painter::put(char c)
{
	canvas_->engine().put_char(c);
	return;
}

void
Painter::put(const std::string& s)
{
	canvas_->engine().put_string(s);
	return;
}

void
Painter::show_cursor(bool state)
{
	if(state) {
		canvas_->engine().show_cursor();
	} else {
		canvas_->engine().hide_cursor();
	}
	return;
}

} // namespace mcurses